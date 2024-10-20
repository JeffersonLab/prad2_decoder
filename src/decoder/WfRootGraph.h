#pragma once

#include "WfAnalyzer.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TSpectrum.h"
#include "TStyle.h"
#include "TAxis.h"
#include <algorithm>
#include <string>
#include <vector>
#include <random>

#define EMPTY_SAMPLES 64

namespace fdec {

struct WfRootConfig
{
    bool show_tspec = false;
    int pm_pos_style = 23;
    double pm_pos_size = 1.0;
    int pm_neg_style = 22;
    double pm_neg_size = 1.0;
    int raw_line_style = 2;
    int raw_line_width = 1;
    int raw_line_color = kRed + 1;
    int res_line_style = 1;
    int res_line_width = 2;
    int res_line_color = kBlue + 1;
    int ped_line_color = kBlack;
    int ped_line_width = 2;
    int ped_line_style = 1;
    int ped_shade_color = kBlack;
    double ped_shade_alpha = 0.3;
    int ped_shade_style = 3000;
    double int_shade_alpha = 0.3;
    int int_shade_style = 3000;
    std::vector<int> int_shade_color = {2};
};

struct LegendEntry
{
    TObject *obj;
    std::string label, option;
};

class WfRootGraph
{
public:
    TMultiGraph *mg;
    std::vector<TObject*> objs;
    std::vector<LegendEntry> entries;
    Fadc250Data result;
};

WfRootGraph get_waveform_graph(const Analyzer &ana, const std::vector<uint32_t> &samples,
                               WfRootConfig cfg = WfRootConfig())
{
    WfRootGraph res;

    res.mg = new TMultiGraph();
    if (samples.empty()) {
        auto gr = new TGraph;
        gr->SetPoint(0, 0, 0);
        gr->SetPoint(1, EMPTY_SAMPLES - 1, 0);
        res.mg->Add(gr, "l");
        res.objs.push_back(dynamic_cast<TObject*>(gr));
        return res;
    }

    // raw waveform
    auto wf = new TGraph(samples.size());
    for (size_t i = 0; i < samples.size(); ++i) {
        wf->SetPoint(i, i, samples[i]);
    }
    wf->SetLineColor(cfg.raw_line_color);
    wf->SetLineWidth(cfg.raw_line_width);
    wf->SetLineStyle(cfg.raw_line_style);
    res.mg->Add(wf, "l");
    res.objs.push_back(dynamic_cast<TObject*>(wf));
    res.entries.emplace_back(LegendEntry{wf, "Raw Samples", "l"});

    // smoothed waveform
    auto wf2 = new TGraph(samples.size());
    auto buf = ana.SmoothSpectrum(&samples[0], samples.size(), ana.GetResolution());
    for (size_t i = 0; i < buf.size(); ++i) {
        wf2->SetPoint(i, i, buf[i]);
    }
    wf2->SetLineColor(cfg.res_line_color);
    wf2->SetLineWidth(cfg.res_line_width);
    wf2->SetLineStyle(cfg.res_line_style);
    res.mg->Add(wf2, "l");
    res.objs.push_back(dynamic_cast<TObject*>(wf2));
    res.entries.emplace_back(LegendEntry{wf2, Form("Smoothed Samples (res = %zu)", ana.GetResolution()), "l"});

    // peak finding
    res.result = ana.Analyze(&samples[0], samples.size());
    auto ped = res.result.ped;
    auto peaks = res.result.peaks;
    auto grm_pos = new TGraph();
    auto grm_neg = new TGraph();

    auto &icolors = cfg.int_shade_color;
    for (size_t i = 0; i < peaks.size(); ++i) {
        // peak amplitudes
        auto peak = peaks[i];
        double range = wf->GetYaxis()-> GetXmax() - wf->GetYaxis()-> GetXmin();
        double height = peak.height + ped.mean + (peak.height > 0 ? 1. : -1.5)*range*0.02;
        if (peak.height >= 0) {
            grm_pos->SetPoint(grm_pos->GetN(), peak.pos, height);
        } else {
            grm_neg->SetPoint(grm_neg->GetN(), peak.pos, height);
        }

        // peak integrals
        auto color = (icolors.size() > i) ? icolors[i]
                     : *std::max_element(icolors.begin(), icolors.end()) + (i + 1 - icolors.size());
        auto nint = peak.right - peak.left + 1;
        auto grs = new TGraph(2*nint);
        for (size_t i = 0; i < nint; ++i) {
            auto val = buf[i + peak.left];
            grs->SetPoint(i, i + peak.left, val);
            grs->SetPoint(nint + i, peak.right - i, ped.mean);
        }
        grs->SetFillColorAlpha(color, cfg.int_shade_alpha);
        grs->SetFillStyle(cfg.int_shade_style);
        res.mg->Add(grs, "f");
        res.objs.push_back(dynamic_cast<TObject*>(grs));
        if (i == 0) {
            res.entries.emplace_back(LegendEntry{grs, "Peak Integrals", "f"});
        }
    }
    grm_pos->SetMarkerStyle(cfg.pm_pos_style);
    grm_pos->SetMarkerSize(cfg.pm_pos_size);
    if (grm_pos->GetN()) { res.mg->Add(grm_pos, "p"); }
    res.objs.push_back(dynamic_cast<TObject*>(grm_pos));
    res.entries.emplace_back(LegendEntry{grm_pos, "Peaks", "p"});

    grm_neg->SetMarkerStyle(cfg.pm_neg_style);
    grm_neg->SetMarkerSize(cfg.pm_neg_size);
    if (grm_neg->GetN()) { res.mg->Add(grm_neg, "p"); }
    res.objs.push_back(dynamic_cast<TObject*>(grm_neg));

    // pedestal line
    auto grp = new TGraphErrors(samples.size());
    for (size_t i = 0; i < samples.size(); ++i) {
        grp->SetPoint(i, i, ped.mean);
        grp->SetPointError(i, 0, ped.err);
    }
    grp->SetFillColorAlpha(cfg.ped_shade_color, cfg.ped_shade_alpha);
    grp->SetFillStyle(cfg.ped_shade_style);
    grp->SetLineStyle(cfg.ped_line_style);
    grp->SetLineWidth(cfg.ped_line_width);
    grp->SetLineColor(cfg.ped_line_color);
    res.mg->Add(grp, "l3");
    res.objs.push_back(dynamic_cast<TObject*>(grp));
    res.entries.emplace_back(LegendEntry{grp, "Pedestal", "lf"});

    // TSpectrum background
    if (cfg.show_tspec) {
        std::vector<double> tped(samples.size());
        tped.assign(samples.begin(), samples.end());
        TSpectrum s;
        s.Background(&tped[0], samples.size(), samples.size()/4, TSpectrum::kBackDecreasingWindow,
                     TSpectrum::kBackOrder2, false, TSpectrum::kBackSmoothing3, false);
        auto grp2 = new TGraph(samples.size());
        for (size_t i = 0; i < samples.size(); ++i) {
            grp2->SetPoint(i, i, tped[i]);
        }
        grp2->SetLineStyle(2);
        grp2->SetLineWidth(2);
        grp2->SetLineColor(kBlack);
        res.mg->Add(grp2, "l");
        res.objs.push_back(dynamic_cast<TObject*>(grp2));
        res.entries.emplace_back(LegendEntry{grp2, "TSpectrum Background", "l"});
    }

    return res;
}

} // namespace fdec
