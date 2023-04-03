#include <stdio.h>
#include <EdbDataSet.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>

int main(int argc, char *argv[])
{
    TCanvas *c1 = new TCanvas();
    TCanvas *c2 = new TCanvas();
    c1->SetLogy();
    c2->SetLogy();
    TH1F *hx = new TH1F("X_Misalignment", "X_Misalignment", 800, -40, 40);
    hx->GetXaxis()->SetTitle("um");
    hx->GetYaxis()->SetTitle("Entries");
    TH1F *hy = new TH1F("Y_Misalignment", "Y_Misalignment", 900, -45, 45);
    hy->GetXaxis()->SetTitle("um");
    hy->GetYaxis()->SetTitle("Entries");

    if (argc <= 1)
    {
        printf("Usage : myanalysis lnk.def\n");
        return 1;
    }

    EdbDataProc *dproc = new EdbDataProc(argv[1]);
    dproc->InitVolume(100, "nseg>=4&&s[0].ePID<=3");
    EdbPVRec *pvr = dproc->PVR();

    // Loop over the tracks
    int ntrk = pvr->Ntracks();
    for (int itrk = 0; itrk < ntrk; itrk++)
    {
        EdbTrackP *t = pvr->GetTrack(itrk);
        // printf("itrk = %d, nseg = %d\n", t->ID(), t->N());

        // Loop over the segments in the track
        int nseg = t->N();
        TGraph x_graph;
        TGraph y_graph;
        for (int iseg = 0; iseg < nseg; iseg++)
        {
            EdbSegP *s = t->GetSegment(iseg);
            x_graph.SetPoint(iseg, s->Z(), s->X());
            y_graph.SetPoint(iseg, s->Z(), s->Y());

            // printf("%8d %3d %3d %8.1f %8.1f %8.1f %7.4f %7.4f\n", s->ID(), s->Plate(), s->PID(), s->X(), s->Y(), s->Z(), s->TX(), s->TY());
        }
        x_graph.Fit("pol1", "Q");
        y_graph.Fit("pol1", "Q");

        for (int iseg = 0; iseg < nseg; iseg++)
        {
            EdbSegP *s = t->GetSegment(iseg);
            double delta_x = s->X() - x_graph.GetFunction("pol1")->Eval(s->Z());
            double delta_y = s->Y() - y_graph.GetFunction("pol1")->Eval(s->Z());
            // printf("%lf %lf\n",delta_x,delta_y);
            hx->Fill(delta_x);
            hy->Fill(delta_y);
        }
    }
    c1->cd();
    hx->Draw();
    c2->cd();
    hy->Draw();
    c1->Print("positonXL1R0_LargePeak.pdf");
    c2->Print("positonYL1R0_LargePeak.pdf");
    return 0;
}