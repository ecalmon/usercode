//#include "TAxis.h"
//#include "TFile.h"
//#include "TGraphAsymmErrors.h"
//#include "TString.h"

#include "autozoom.C"
#include "drawGraphEnv.C"
//#include "tdrstyle.C"
#include "TMath.h"

void pf_macro(char * filename = "out_plot_GR_R_42_V19::All_EcalLaserAPDPNRatios_v3_online.root"){
  all(filename);
}

void ovfIntoBins(TH1* h){
  TAxis* a = h->GetXaxis();
  int nbins = a->GetNbins();
  h->SetBinContent(1, h->GetBinContent(1) + h->GetBinContent(0));
  h->SetBinContent(0,0);
  h->SetBinContent(nbins, h->GetBinContent(nbins) + h->GetBinContent(nbins+1));
  h->SetBinContent(nbins+1, 0);
}

template<class T>
void draw(T* h, const char* opt = "")
{
  //   return;
  //fix style for graph:
  TAxis * a[2];
  char* aname[] = { "X", "Y"};
  a[0] = h->GetXaxis();
  a[1] = h->GetYaxis();
  for(unsigned i = 0; i < sizeof(a)/sizeof(a[0]); ++i){
    a[i]->SetTitleFont(gStyle->GetTitleFont(aname[i]));
    a[i]->SetTitleSize(gStyle->GetTitleSize(aname[i]));
    a[i]->SetTitleColor(gStyle->GetTitleColor(aname[i]));
    a[i]->SetLabelFont(gStyle->GetLabelFont(aname[i]));
    a[i]->SetLabelSize(gStyle->GetLabelSize(aname[i]));
    a[i]->SetLabelColor(gStyle->GetLabelColor(aname[i]));
  }
  h->GetXaxis()->SetTitleOffset(gStyle->GetTitleXOffset());
  h->GetYaxis()->SetTitleOffset(gStyle->GetTitleYOffset());
  
  h->Draw(opt);
}

void gplot(TFile * f, char * gname, char * title = 0)
{
    const char * nfrac[] = { "3S", "2S", "1S", "E" };
    const char * nleg[] = { "99.7% of channels", "95.4% of channels", "68.2% of channels", "extrema #times 0.1" };
    //const int ncol[] = { kOrange + 1, TColor::GetColor("#ffff00"), TColor::GetColor("#33ee33"), 0 };
    const int ncol[] = { kOrange + 1, TColor::GetColor("#ffff00"), 8, 0 };
    char str[64];
    TGraphAsymmErrors * g = 0;
    TH1 * gg = 0;

    TLatex  * t = new TLatex(0.87, 0.8, title);
    t->SetTextAlign(21);
    t->SetNDC();
    t->SetTextFont(42);
    t->SetTextSize(0.06);
    TLegend * l = new TLegend(0.775, 0.13, 0.95, 0.13 + 0.35);

    for (int i = 0; i < sizeof(nfrac)/sizeof(char*); ++i) {
        sprintf(str, "%s_%s", gname, nfrac[i]);
	//        printf("--> %s\n", str);
        //g = (TGraphAsymmErrors*)gDirectory->Get(str);
        g = (TGraphAsymmErrors*)f->Get(str);
	//        printf("--> %p\n", g);
        g->SetMarkerStyle(20);
        g->SetMarkerSize(.5);
        g->SetFillColor(ncol[i]);
        //draw(g, i == 0 ? "ae3" : "e3");
        if (i != 3) draw(g, i == 0 ? "ae3" : "e3");
        else {
            drawGraphEnv(g, 0.1, false, 7, 2, 1, false, &gg);
        }
        if (i == 3) draw(g, "xl");
        g->GetXaxis()->SetTimeDisplay(1);
	g->GetXaxis()->SetNdivisions(505);
        g->GetXaxis()->SetTitle("time");
        g->GetYaxis()->SetTitle("transparency change");
        //g->GetYaxis()->SetRangeUser(0.5, 1.15);
        gPad->SetTicks();
        //if (title) g->SetTitle(title);
        if (i == 0) draw(g, i == 0 ? "ae3" : "e3");
        if (i == 0) l->AddEntry(g, "median", "l");
        l->AddEntry(i == 3 ? gg : g, nleg[i], i == 3 ? "l" : "f");
    }
    l->SetFillStyle(0);
    l->SetBorderSize(0);
    l->SetTextFont(42);
    l->SetTextSize(0.0475);
    l->Draw();
    t->Draw();
    gPad->Print((std::string(gname) + ".eps").c_str());
    //        gPad->Print("anim.gif+10");
}

void setStyle(){
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetTitleFont(42, "XYZ");
  gStyle->SetTitleSize(0.055, "XYZ");
  gStyle->SetTitleOffset(1., "X");
  gStyle->SetTitleOffset(.75, "Y");
  gStyle->SetLabelFont(42, "XYZ");
  gStyle->SetLabelSize(0.055, "XYZ");
  
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.13);
  gStyle->SetPadLeftMargin(0.10);
  gStyle->SetPadRightMargin(0.26);
  
  gStyle->SetHatchesLineWidth(2.);
  
  gStyle->SetTimeOffset(0);
  
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
}

TH1* getErrorHist(const TH1* h){
  TH1* he = (TH1*) h->Clone();
  he->Reset();
  int nx = h->GetNbinsX();
  if(nx>1) nx += 2;
  int ny = h->GetNbinsY();
  if(ny>1) ny += 2;
  int nz = h->GetNbinsZ();
  if(nz>1) nz += 2;
  for(int ix = 0; ix < nx; ++ix){
    for(int iy = 0; iy < nx; ++iy){
      for(int iz = 0; iz < nx; ++iz){
	double e = h->GetBinError(ix,iy,iz);
	cout << ix << "," << iy << "," << iz << ": " << e << endl;
	he->SetBinContent(ix, iy, iz, e);
      }
    }
  }
  return he;
}

void drawNormP2Hist(){
   TH1* hp2norm = (TH1*) gDirectory->Get("distr_eta_normalised_p2");
    if(hp2norm){
      hp2norm->GetXaxis()->SetTitle("tranps./<transp.>_{#eta}");
      hp2norm->GetYaxis()->SetTitle("entries (channel#timesIOV)");
      TCanvas* c = new TCanvas(TString("c_") + hp2norm->GetName(), hp2norm->GetTitle(), 800, 400);
      c->SetLogy();
      //      ovfIntoBins(hp2norm);
      gStyle->SetStatBorderSize(0);
      gStyle->SetOptStat("ou");
	
      draw(hp2norm);

      c->Update();
      TPaveStats* ps = (TPaveStats*) hp2norm->GetListOfFunctions()->FindObject("stats");
      if(ps){
	ps->SetX1NDC(1.30e-01);
	ps->SetX2NDC(2.7e-01);
	ps->SetY1NDC(8.467e-01);
	ps->SetY2NDC(9.274e-01);
	ps->Draw();
      } else{
	cerr << "Statistics pave of " << hp2norm->GetName() << " was not found." << endl;
      }

      TPad* p = new TPad("p","p", .5, .5, .99-c->GetRightMargin(), .99-c->GetTopMargin());
      p->SetRightMargin(0.05);
      p->SetTopMargin(0.09);
      p->Draw();
      p->cd();
      TH1* h = hp2norm->Clone(TString(hp2norm->GetName()) + "_lin");
      h->SetStats(0);

      h->GetXaxis()->SetRangeUser(0.95, 1.05);
      h->GetXaxis()->SetNdivisions(505);
      h->GetXaxis()->SetTitle("");
      h->GetYaxis()->SetTitle("");
      draw(h);
      h->GetYaxis()->SetLabelSize(0.07);
      h->GetXaxis()->SetLabelSize(0.07);

      c->cd();

      TLatex  * t = new TLatex(0.87, 0.8, "Transparency");
      t->SetTextAlign(21);
      t->SetNDC();
      t->SetTextFont(42);
      t->SetTextSize(0.06);
      t->Draw();
      t->DrawLatex(0.87, 0.74, "normalized to the");
      t->DrawLatex(0.87, 0.68, "eta-ring average");

      c->Print(TString(hp2norm->GetName()) + ".eps");
    }
    //restore our default style:
    setStyle();
}

void drawNormP2Map(){
  const char* names[] = { "EEprof2_nZ_transpCorrNorm", "EBprof2_transpCorrNorm", "EEprof2_pZ_transpCorrNorm"};
  const char* part[]  = { "EE-", "EB", "EE+" };
  TCanvas* c = new TCanvas("prof", "prof", 800, 400);
  c->SetLogz();
  c->SetLeftMargin(0.07);
  for(int i = 0; i < sizeof(names)/sizeof(names[0]); ++i){
    cout << names[i] << endl;
    TProfile2D* h = (TProfile2D*) gDirectory->Get(names[i]);
    if(!h) continue;
    TH2* h2[2];
    h2[0] = (TH2*) h;
    h2[1]= h->ProjectionXY(TString(h->GetName()) + "_RMS", "C=E");
    char* type[] = {"Mean", "RMS"};
    for(int j = 0; j < 2; ++j){
      c->SetLogz(j);
      h2[j]->Draw("colz");

      c->Update();
      
      TPaletteAxis *palette = (TPaletteAxis*)h2[j]->GetListOfFunctions()->FindObject("palette");

      if(palette){
	palette->SetX2NDC(0.5*palette->GetX1NDC() + 0.5*palette->GetX2NDC());
	palette->SetY2NDC(0.3*palette->GetY1NDC() + 0.7*palette->GetY2NDC());
	//	palette->ConvertNDCtoPad();
	palette->Paint("NDC");
      } else{
	cerr << "Palette not found!" << endl;
      }
    
      h2[j]->GetXaxis()->SetTitle(i==1 ? "iphi" : "iy");
      h2[j]->GetYaxis()->SetTitle(i==1 ? "ieta" : "ix");

      if(j==0) autozoom((TH2F*)h2[j]);

      c->Paint();
      
      TLatex  * t = new TLatex(0.87, 0.9, "Transparency");
      t->SetTextAlign(21);
      t->SetNDC();
      t->SetTextFont(42);
      t->SetTextSize(0.06);
      t->Draw();
      TLatex* t2 = (TLatex*) t->Clone();
      t2->SetTextColor(kRed+2);
      t->DrawLatex(0.87, 0.84, "normalized to the");
      t->DrawLatex(0.87, 0.78, "eta-ring average");
      t->DrawLatex(0.87, 0.69, type[j]);
      t2->DrawLatex(0.9, 0.2, part[i]);
      c->Print(TString(h2[j]->GetName()) + ".eps");
    }
  } 
}

int all(char * filename = "out_plot_GR_R_42_V19::All_EcalLaserAPDPNRatios_v3_online.root")
{

  setStyle();

  printf("opening file %s\n", filename);
  TFile * fin = TFile::Open(filename);

  drawNormP2Hist();

  drawNormP2Map();

  TCanvas * c = new TCanvas("history", "history", 800, 400);
  if (!fin || fin->IsZombie()) {
    return 0;
  }
  
  gplot(fin, "history_p2_All", "All ECAL");
  gplot(fin, "history_p2_EE-", "EE-");
  gplot(fin, "history_p2_EB-", "EB-");
  gplot(fin, "history_p2_EB+", "EB+");
  gplot(fin, "history_p2_EE+", "EE+");
    
    
    char buf1[256];
    char buf2[256];
    for(int i=1; i <= 92; ++i){
      sprintf(buf1, "LM%02d", i);
      sprintf(buf2, "history_p2_%s", buf1);
      gplot(fin, buf2, buf1);
    }
    
    const int netabins = 20;
    const float deta = 2*2.964 / netabins;
    for(int i=1; i <= 20; ++i){
      float etamin = -2.964 + (i-1) * deta;
      float etamax = etamin + deta;
      etamin = TMath::Nint(etamin  * 100) / 100.;
      etamax = TMath::Nint(etamax  * 100) / 100.;
      sprintf(buf1, "%.2g < eta < %.2g", etamin, etamax);
      sprintf(buf2, "history_p2_eta%02d", i);
      gplot(fin, buf2, buf1);
    }

    //         gPad->Print("anim.gif++");

    TProfile2D * pm = (TProfile2D*)gDirectory->Get("EBprof2_transpCorr_week");
    TIter next(gDirectory->GetListOfKeys());
    int n, t;
    while(1) {
        TObject * o = next();
        if (o == 0) break;
        if ((n = sscanf(o->GetName(), "EBprof2_transpCorr_week_%d", &t)) == 1) {
            printf("%s\n", o->GetName());
            TProfile2D * p = (TProfile2D*)gDirectory->Get(o->GetName());
            autozoom((TH2F*)p);
            p->Draw("colz");
            gPad->Print((std::string(o->GetName()) + ".png").c_str());
        }
    }
}
