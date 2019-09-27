#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>

#include <sys/stat.h>
#include "doReconstruct.C"

class MyMainFrame {
   RQ_OBJECT("MyMainFrame")
private:
   TGMainFrame           *fMain;
   
   TRootEmbeddedCanvas *fCanvas1 = nullptr;
   TRootEmbeddedCanvas *fCanvas2 = nullptr;
   TRootEmbeddedCanvas *fCanvas3 = nullptr;
   TRootEmbeddedCanvas *fCanvas4 = nullptr;   
   
   TH2I *fPrimHist = nullptr;

   TGTextButton         *fStartBut;
   TGTextButton      *fSetParamBut;
   
   TGTextEntry        *fDiskREnt = nullptr;
   TGTextEntry       *fNsipmsEnt = nullptr;
   TGTextEntry    *fPixelSizeEnt = nullptr;
   TGTextEntry        *fGammaEnt = nullptr;
   TGTextEntry  *fUnpenalizedIterEnt = nullptr;
   TGTextEntry    *fPenalizedIterEnt = nullptr;

   TGVButtonGroup   *dataTypeButGroup;
   TGCheckButton             *isMCBut;
   TGCheckButton           *isDataBut;

   TGVButtonGroup       *emmlButGroup;

   TGCheckButton           *doEmMlBut;
   TGCheckButton           *doChi2But;
   TGCheckButton      *doPenalizedBut;

   std::string fTopDir;
   bool        fIsRunning  = false;
   double      fLastUpdate = 0;
   TTimer      *fTimer     = nullptr;
   std::string fDataFilePath = "./daq/data.txt";
   std::string fTrueDistPath = "../output/simulateOutput.root";
   double      fDiskR      = 50.561875;
   int         fNsipms     = 64;
   int         fPixelSize  = 5;
   size_t      fUnpenalizedIter = 100;
   size_t      fPenalizedIter   = 100;
   double      fGamma      = 0.5;
   std::string fDataType   = "data";
   std::string fMethod     = "chi2";
   RecoHelper  fRecoHelper;

public:
   MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h, std::string topDir);
   virtual ~MyMainFrame();
   void DoDraw();
   void ChangeStartLabel();
   void StartReco();
   void StopReco();
   void SetParameters();
   bool IsDAQFileModified();
   void HandleTimer();
   const std::map<size_t, size_t> ReadDataFile();
   void UpdatePlots();
   void SetDataTypeMC();
   void SetDataTypeData();
   void SetMethodChi2();
   void SetMethodEmMl();
   void UpdateRecoHelper();
};

//------------------------------------------------------------------------
MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h, std::string topDir) 
{
   // Create a main frame
   fMain = new TGMainFrame(p,w,h);

   fTopDir = topDir;

   // Create a horizontal frame
   TGHorizontalFrame *hframe1 = new TGHorizontalFrame(fMain,1500,1000);

   // Create vertical frames
   TGVerticalFrame *vframe1 = new TGVerticalFrame(hframe1,400,1000);
   TGVerticalFrame *vframe2 = new TGVerticalFrame(hframe1,800,1000);
   TGVerticalFrame *vframe3 = new TGVerticalFrame(hframe1,400,1000);

   vframe1->Resize(200,1000);
   hframe1->AddFrame(vframe1, new TGLayoutHints(kLHintsLeft | kLHintsExpandY,5,5,30,100));

   // horizontal frames for canvases
   TGHorizontalFrame *hframe3 = new TGHorizontalFrame(fMain,400,1000);
   fCanvas1 = new TRootEmbeddedCanvas("fCanvas1",hframe3,200,200);   
   hframe3->AddFrame(fCanvas1, new TGLayoutHints(kLHintsExpandX | kLHintsCenterX | kLHintsCenterY | kLHintsExpandY,5,5,5,5)); 
   fCanvas2 = new TRootEmbeddedCanvas("fCanvas2",hframe3,200,200);
   hframe3->AddFrame(fCanvas2, new TGLayoutHints(kLHintsExpandX | kLHintsCenterX | kLHintsCenterY | kLHintsExpandY,5,5,5,5));

   TGHorizontalFrame *hframe4 = new TGHorizontalFrame(fMain,400,1000);
   fCanvas3 = new TRootEmbeddedCanvas("fCanvas3",hframe4,200,200);   
   hframe4->AddFrame(fCanvas3, new TGLayoutHints(kLHintsExpandX | kLHintsCenterX | kLHintsCenterY | kLHintsExpandY,5,5,5,5)); 
   fCanvas4 = new TRootEmbeddedCanvas("fCanvas4",hframe4,200,200);
   hframe4->AddFrame(fCanvas4, new TGLayoutHints(kLHintsExpandX | kLHintsCenterX | kLHintsCenterY | kLHintsExpandY,5,5,5,5));

   vframe2->AddFrame(hframe3, new TGLayoutHints(kLHintsExpandX | kLHintsCenterX | kLHintsCenterY | kLHintsExpandY,5,5,5,5)); 
   vframe2->AddFrame(hframe4, new TGLayoutHints(kLHintsExpandX | kLHintsCenterX | kLHintsCenterY | kLHintsExpandY,5,5,5,5)); 
   vframe2->Resize(1100,1000);
   hframe1->AddFrame(vframe2, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsCenterY | kLHintsExpandY, 5,5,5,5) );

   // bold font for labels
   TGGC *fTextGC;
   const TGFont *boldfont = gClient->GetFont("-*-times-bold-r-*-*-18-*-*-*-*-*-*-*");
   if (!boldfont) boldfont = gClient->GetResourcePool()->GetDefaultFont();
   FontStruct_t labelboldfont = boldfont->GetFontStruct();
   GCValues_t   gval;
   gval.fMask = kGCBackground | kGCFont | kGCForeground;
   gval.fFont = boldfont->GetFontHandle();
   gClient->GetColorByName("yellow", gval.fBackground);
   fTextGC = gClient->GetGC(&gval, kTRUE);

   TGHorizontalFrame *vhframe20 = new TGHorizontalFrame(vframe3,400,20);
   TGLabel *title = new TGLabel(vhframe20, "SiPM Wheel\nEvent Display", fTextGC->GetGC(), labelboldfont, kChildFrame);
   vhframe20->AddFrame(title, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 50, 5));
   //title->SetTextColor(blackcolor);

   /* majorana picture */
   TRootEmbeddedCanvas *majPicCanvas = new TRootEmbeddedCanvas("majPicCanvas",vhframe20,100,100);
   vhframe20->AddFrame(majPicCanvas, new TGLayoutHints(kLHintsTop | kLHintsRight,5,5,5,5));
   TImage *etimg = TImage::Open("evd/et.png");
   if (!etimg) {
      printf("Could not find image... \n");
   }
   else {
     etimg->SetConstRatio(0);
     etimg->SetImageQuality(TAttImage::kImgBest);
     TCanvas* c = majPicCanvas->GetCanvas();
     c->cd();
     c->Clear();
     c->SetLineColor(0);
     etimg->Draw("xxx");
     c->Update();
   }
   vframe3->AddFrame(vhframe20, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 50, 5));

   /* Reco Parameters */
   TGGC *fTextGC1;
   const TGFont *regfont = gClient->GetFont("-*-adobe-r-*-*-12-*-*-*-*-*-*-*");
   if (!regfont) {
        regfont = gClient->GetResourcePool()->GetDefaultFont();
   }
   FontStruct_t label1font = regfont->GetFontStruct();
   GCValues_t   gval1;
   gval1.fMask = kGCBackground | kGCFont | kGCForeground;
   gval1.fFont = regfont->GetFontHandle();
   gClient->GetColorByName("black", gval1.fBackground);
   fTextGC1 = gClient->GetGC(&gval1, kTRUE);
   //gClient->GetColorByName("black", blackcolor);
   TGLabel *tRecoConfig = new TGLabel(vframe3, "Configuration", fTextGC->GetGC(), labelboldfont, kChildFrame);
   vframe3->AddFrame(tRecoConfig, new TGLayoutHints(kLHintsCenterX, 5,5,30,5));

   /* Disk Radius */
   stringstream stream;
   stream << fixed << setprecision(2) << fDiskR;
   TGHorizontalFrame *vhframe6 = new TGHorizontalFrame(vframe3,400,20);
   vhframe6->AddFrame(new TGLabel(vhframe6, "Disk Radius: ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsLeft, 0,5,5,5));
   TGTextBuffer *diskRBuf = new TGTextBuffer(10);
   diskRBuf->AddText(0, stream.str().c_str());
   fDiskREnt = new TGTextEntry(vhframe6, diskRBuf);
   fDiskREnt->Resize(50, fDiskREnt->GetDefaultHeight());
   fDiskREnt->SetFont("-adobe-courier-r-*-*-12-*-*-*-*-*-iso8859-1");
   vhframe6->AddFrame(new TGLabel(vhframe6, "cm", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsRight, 5,5,5,0));
   vhframe6->AddFrame(fDiskREnt, new TGLayoutHints(kLHintsRight | kLHintsTop,5,5,2,5));
   vframe3->AddFrame(vhframe6, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,5,5));

   /* N sipms */  
   TGHorizontalFrame *vhframe10 = new TGHorizontalFrame(vframe3,400,20);
   vhframe10->AddFrame(new TGLabel(vhframe10, "N SiPMs: ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsLeft, 0,5,5,5));
   TGTextBuffer *nsipms = new TGTextBuffer(10);
   nsipms->AddText(0, std::to_string(fNsipms).c_str());
   fNsipmsEnt = new TGTextEntry(vhframe10, nsipms);
   fNsipmsEnt->Resize(50, fNsipmsEnt->GetDefaultHeight());
   fNsipmsEnt->SetFont("-adobe-courier-r-*-*-12-*-*-*-*-*-iso8859-1");
   vhframe10->AddFrame(new TGLabel(vhframe10, "  ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsRight, 5,5,5,0));
   vhframe10->AddFrame(fNsipmsEnt, new TGLayoutHints(kLHintsRight | kLHintsTop,5,5,2,5));
   vframe3->AddFrame(vhframe10, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,5,5));

   /* pixel size */   
   stream.str("");
   stream << fixed << setprecision(2) << fPixelSize;
   TGHorizontalFrame *vhframe16 = new TGHorizontalFrame(vframe3,400,20);
   vhframe16->AddFrame(new TGLabel(vhframe16, "Pixel spacing: ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsLeft, 0,5,5,5));
   TGTextBuffer *pixelsize = new TGTextBuffer(10);
   pixelsize->AddText(0, stream.str().c_str());
   fPixelSizeEnt = new TGTextEntry(vhframe16, pixelsize);
   fPixelSizeEnt->Resize(50, fPixelSizeEnt->GetDefaultHeight());
   fPixelSizeEnt->SetFont("-adobe-courier-r-*-*-12-*-*-*-*-*-iso8859-1");
   vhframe16->AddFrame(new TGLabel(vhframe16, "mm", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsRight, 5,5,5,0));
   vhframe16->AddFrame(fPixelSizeEnt, new TGLayoutHints(kLHintsRight | kLHintsTop,5,5,2,5));
   vframe3->AddFrame(vhframe16, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,5,5));

   /* Method */   
   TGHorizontalFrame *vhframe17 = new TGHorizontalFrame(vframe3,400,20);
   vhframe17->AddFrame(new TGLabel(vhframe17, "Method: ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsLeft, 0,5,5,5));
   doChi2But = new TGCheckButton(vhframe17, new TGHotString("Chi2"));
   vhframe17->AddFrame(doChi2But, new TGLayoutHints(kLHintsRight, 5,5,5,5));
   doChi2But->SetState(kButtonDown);
   doChi2But->Connect("Clicked()","MyMainFrame",this,"SetMethodChi2()");
   doEmMlBut = new TGCheckButton(vhframe17, new TGHotString("EM-ML"));
   vhframe17->AddFrame(doEmMlBut, new TGLayoutHints(kLHintsRight, 5,5,5,5));
   vframe3->AddFrame(vhframe17, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,5,5));
   if (doChi2But->IsOn()) doEmMlBut->SetState(kButtonUp);
   else doEmMlBut->SetState(kButtonDown);
   doEmMlBut->Connect("Clicked()","MyMainFrame",this,"SetMethodEmMl()");

   /* Gamma */   
   dataTypeButGroup = new TGVButtonGroup(vframe3, "EM-ML Parameters");
   stream.str("");
   stream << fixed << setprecision(2) << fGamma;
   TGHorizontalFrame *vhframe12 = new TGHorizontalFrame(dataTypeButGroup,400,20);
   vhframe12->AddFrame(new TGLabel(vhframe12, "Gamma: ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsLeft, 0,5,5,5));
   TGTextBuffer *gammaBuf = new TGTextBuffer(10);
   gammaBuf->AddText(0, stream.str().c_str());
   fGammaEnt = new TGTextEntry(vhframe12, gammaBuf);
   fGammaEnt->Resize(50, fGammaEnt->GetDefaultHeight());
   fGammaEnt->SetFont("-adobe-courier-r-*-*-12-*-*-*-*-*-iso8859-1");
   vhframe12->AddFrame(new TGLabel(vhframe12, "  ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsRight, 5,5,5,0));
   vhframe12->AddFrame(fGammaEnt, new TGLayoutHints(kLHintsRight | kLHintsTop,5,5,2,5));
   dataTypeButGroup->AddFrame(vhframe12, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,5,5));

   /* Do penalized reconstruction */   
   TGHorizontalFrame *vhframe11 = new TGHorizontalFrame(dataTypeButGroup,400,20);
   vhframe11->AddFrame(new TGLabel(vhframe11, "Do penalized: ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsLeft, 0,5,5,5));
   doPenalizedBut = new TGCheckButton(vhframe11, new TGHotString(""));
   vhframe11->AddFrame(doPenalizedBut, new TGLayoutHints(kLHintsRight, 5,40,5,5));
   dataTypeButGroup->AddFrame(vhframe11, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,5,5));
   doPenalizedBut->SetState(kButtonDown);

   /* Unpenalized iter */   
   TGHorizontalFrame *vhframe14 = new TGHorizontalFrame(dataTypeButGroup,400,20);
   vhframe14->AddFrame(new TGLabel(vhframe14, "Unpenalized iterations: ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsLeft, 0,5,5,5));
   TGTextBuffer *unpenIter = new TGTextBuffer(10);
   unpenIter->AddText(0, std::to_string(fUnpenalizedIter).c_str());
   fUnpenalizedIterEnt = new TGTextEntry(vhframe14, unpenIter);
   fUnpenalizedIterEnt->Resize(50, fUnpenalizedIterEnt->GetDefaultHeight());
   fUnpenalizedIterEnt->SetFont("-adobe-courier-r-*-*-12-*-*-*-*-*-iso8859-1");
   vhframe14->AddFrame(new TGLabel(vhframe14, "  ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsRight, 5,5,5,0));
   vhframe14->AddFrame(fUnpenalizedIterEnt, new TGLayoutHints(kLHintsRight | kLHintsTop,5,5,2,5));
   dataTypeButGroup->AddFrame(vhframe14, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,5,5));

   /* Unpenalized iter */   
   TGHorizontalFrame *vhframe15 = new TGHorizontalFrame(dataTypeButGroup,400,20);
   vhframe15->AddFrame(new TGLabel(vhframe15, "Penalized iterations: ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsLeft, 0,5,5,5));
   TGTextBuffer *penIter = new TGTextBuffer(10);
   penIter->AddText(0, std::to_string(fPenalizedIter).c_str());
   fPenalizedIterEnt = new TGTextEntry(vhframe15, penIter);
   fPenalizedIterEnt->Resize(50, fPenalizedIterEnt->GetDefaultHeight());
   fPenalizedIterEnt->SetFont("-adobe-courier-r-*-*-12-*-*-*-*-*-iso8859-1");
   vhframe15->AddFrame(new TGLabel(vhframe15, "  ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsRight, 5,5,5,0));
   vhframe15->AddFrame(fPenalizedIterEnt, new TGLayoutHints(kLHintsRight | kLHintsTop,5,5,2,5));
   dataTypeButGroup->AddFrame(vhframe15, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,5,5));

   vframe3->AddFrame(dataTypeButGroup, new TGLayoutHints(kLHintsExpandX,5,5,5,5) );

   /* Set Params */
   fSetParamBut = new TGTextButton(vframe3,"Set Parameters");
   fSetParamBut->Connect("Clicked()","MyMainFrame",this,"SetParameters()");
   vframe3->AddFrame(fSetParamBut, new TGLayoutHints(kLHintsExpandX,5,5,5,5) );

   vframe3->Resize(400,1000);
   hframe1->AddFrame(vframe3, new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 5,5,5,5) );

   /* Data type buttons */
   dataTypeButGroup = new TGVButtonGroup(vframe3, "Data Type");
   isDataBut = new TGCheckButton(dataTypeButGroup, new TGHotString("Data"));
   isDataBut->Connect("Clicked()","MyMainFrame",this,"SetDataTypeData()");
   isMCBut   = new TGCheckButton(dataTypeButGroup, new TGHotString("MC"));
   isMCBut->Connect("Clicked()","MyMainFrame",this,"SetDataTypeMC()");
   vframe3->AddFrame(dataTypeButGroup, new TGLayoutHints(kLHintsExpandX, 5,5,5,5) );
   fDataType == "data" ? isDataBut->SetState(kButtonDown) : isMCBut->SetState(kButtonDown);


   /* UTA picture */
   TRootEmbeddedCanvas *utaPicCanvas = new TRootEmbeddedCanvas("utaPicCanvas",vframe3,150,75);
   vframe3->AddFrame(utaPicCanvas, new TGLayoutHints(kLHintsBottom | /*kLHintsExpandX |*/ kLHintsCenterX | kLHintsCenterY,5,5,5,25));
   TImage *img = TImage::Open("evd/utalogo2.jpg");
   if (!img) {
      printf("Could not find image... \n");
   }
   else {
     img->SetConstRatio(0);
     img->SetImageQuality(TAttImage::kImgBest);
     TCanvas* c = utaPicCanvas->GetCanvas();
     c->cd();
     c->Clear();
     c->SetLineColor(0);
     img->Draw("xxx");
     c->Update();
   }

   /* Quit button */
   TGTextButton *quit = new TGTextButton(vframe3,"&Quit", "gApplication->Terminate(0)");
   vframe3->AddFrame(quit, new TGLayoutHints(kLHintsExpandX | kLHintsBottom,5,5,5,100) );

   /* Start button */
   fStartBut = new TGTextButton(vframe3,"Start");
   fStartBut->Connect("Clicked()","MyMainFrame",this,"ChangeStartLabel()");
   vframe3->AddFrame(fStartBut, new TGLayoutHints(kLHintsBottom | kLHintsExpandX ,5,5,5,5));

   TGLabel *tReco = new TGLabel(vframe3, "Reconstruction", fTextGC->GetGC(), labelboldfont, kChildFrame);
   vframe3->AddFrame(tReco, new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 5,5,5,5));

   // Add horizontal to main frame //
   fMain->AddFrame(hframe1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,5,5));
   // Set a name to the main frame //
   fMain->SetWindowName("Event Display");
   // Map all subwindows of main frame //
   fMain->MapSubwindows();
   // Initialize the layout algorithm // 
   fMain->Resize(fMain->GetDefaultSize());
   // Map main frame //
   fMain->MapWindow();
   fMain->Resize(1500, 1000);
   if (!fTimer) {
     fTimer = new TTimer();
     fTimer->Connect("Timeout()", "MyMainFrame", this, "HandleTimer()");
   }
   std::cout << std::endl;
}

//------------------------------------------------------------------------
MyMainFrame::~MyMainFrame() {
  // Clean up used widgets: frames, buttons, layout hints
  fMain->Cleanup();
  delete fMain;
}

//------------------------------------------------------------------------
void MyMainFrame::SetDataTypeData()
{
  fDataType = "data";

  if (isMCBut->IsOn()) isMCBut->SetState(kButtonUp);
  isDataBut->SetState(kButtonDown);
}

//------------------------------------------------------------------------
void MyMainFrame::SetDataTypeMC()
{
  fDataType = "mc";

  if (isDataBut->IsOn()) isDataBut->SetState(kButtonUp);
  isMCBut->SetState(kButtonDown);
}

//------------------------------------------------------------------------
void MyMainFrame::SetMethodChi2()
{
  fMethod = "chi2";

  if (doEmMlBut->IsOn()) doEmMlBut->SetState(kButtonUp);
  doChi2But->SetState(kButtonDown);
}

//------------------------------------------------------------------------
void MyMainFrame::SetMethodEmMl()
{
  fMethod = "emml";

  if (doChi2But->IsOn()) doChi2But->SetState(kButtonUp);
  doEmMlBut->SetState(kButtonDown);
}


//------------------------------------------------------------------------
void MyMainFrame::ChangeStartLabel()
{
  fStartBut->SetState(kButtonDown);

  // If it is not currently running, start it
  if (std::string(*fStartBut->GetText()) == "Start") 
  {
    fStartBut->SetText("Stop");

    // Initialize reco helper
    UpdateRecoHelper();

    // Start the time to check for updated daq file
    std::cout << "\nListening for DAQ files...\n";
    if (fTimer) fTimer->Start(1000, kFALSE);
  } 
  else 
  {
    std::cout << "Stopping reconstruction...\n";
    fStartBut->SetText("Start");
    if (fTimer) fTimer->Stop();
  }
  fStartBut->SetState(kButtonUp);
}

//------------------------------------------------------------------------
void MyMainFrame::UpdateRecoHelper()
{
  std::string pixelizationPath = fTopDir+"/production/mothership/"+std::to_string(fPixelSize)+"mm/pixelization.txt";
  std::string opRefTablePath   = fTopDir+"/production/mothership/"+std::to_string(fPixelSize)+"mm/opRefTable.txt";
  
  // if these are new, we will load the new pixel scheme
  std::string oldPixelPath  = fRecoHelper.thePixelPath;
  std::string oldOpRefPath  = fRecoHelper.theOpRefPath;

  fRecoHelper.theMethod          = fMethod;
  fRecoHelper.thePixelPath       = pixelizationPath;
  fRecoHelper.theOpRefPath       = opRefTablePath;
  fRecoHelper.theDiskRadius      = fDiskR;
  fRecoHelper.theGamma           = fGamma;
  fRecoHelper.theDoPenalized     = doPenalizedBut->IsOn();
  fRecoHelper.thePenalizedIter   = fPenalizedIter;
  fRecoHelper.theUnpenalizedIter = fUnpenalizedIter;
  fRecoHelper.thePixelSpacing    = fPixelSize/10.; // in cm

  // Load pixels if needed
  if (fRecoHelper.thePixelPath != oldPixelPath) LoadPixelization(fRecoHelper);
  if (fRecoHelper.theOpRefPath != oldOpRefPath) LoadOpRefTable(fRecoHelper);
}

//------------------------------------------------------------------------
void MyMainFrame::StartReco() 
{
   std::cout << "//////////////////////////////////////////////////////\n";
   std::cout << "\nStarting reconstruction..." << std::endl;
   fIsRunning = true;
   
   // Read the daq file
   fRecoHelper.theData = ReadDataFile();
   if (fRecoHelper.theData.size() != fNsipms) {cout << "\nWarning: Data size not equal to " << fNsipms << "\n"; return;}

   // Start reconstruction
   Reconstruct(fRecoHelper);

   // Update plots
   UpdatePlots();
   std::cout << "Updated plots...\n";

   // Finished
   fIsRunning = false;
}

//------------------------------------------------------------------------
const std::map<size_t, size_t> MyMainFrame::ReadDataFile() 
{
  // The file should contain the number of photons detected by the sipms
  std::ifstream theFile(fDataFilePath.c_str());
  std::string line;
  std::map<size_t, size_t> v;
  if (theFile.is_open()) {
    // this is data
    std::getline(theFile, line);
  }
  if (!line.size()) return v;

  // remove white space
  size_t pos = 0;
  size_t counter = 1;
  std::string delimiter = " ";
  while ((pos = line.find(delimiter)) != std::string::npos) {
    v.emplace(counter, std::stoi(line.substr(0, pos)));
    line.erase(0, pos + delimiter.length());
    counter++;
  }
  if (line.size() > 1) v.emplace(counter, std::stoi(line));
  //for (const auto& d : v) cout << d.first << " " << d.second << endl;
  return v;
}

//------------------------------------------------------------------------
void MyMainFrame::UpdatePlots() 
{
  ///////////////////
  // Updating canvas 1
  // Grab the top canvas
  TCanvas *c1 = fCanvas1->GetCanvas();
  c1->Clear();

  // We need the resulting plot from reco
  TFile f("recoanatree.root", "READ");
  if (f.IsOpen()) {
    gStyle->SetPalette(kDarkBodyRadiator);
    TH2F *recoHist = nullptr;
    f.GetObject("histFinal", recoHist);
    if (recoHist) {

      cout << "HEYYY " << recoHist->GetMaximum() << endl;
      recoHist->SetTitle("Reconstructed Image");
      recoHist->GetXaxis()->SetTitle("X [cm]");
      recoHist->GetYaxis()->SetTitle("Y [cm]");
      recoHist->Draw("colz");
      if (fDataType == "mc")
      {
        //recoHist->SetMinimum(0);
        //recoHist->SetMaximum(fPrimHist->GetMaximum());
      }
      c1->cd();
      c1->Update();
    } else {cout << "\nWARNING: Couldn't find reco hist...\n";}
  } else {cout << "\nWARNING: Couldn't open root file...\n";}
  ///////////////////

  ///////////////////
  // Updating canvas 2
  TCanvas *c2 = fCanvas2->GetCanvas();
  c2->Clear();
  if (f.IsOpen()) {
    gStyle->SetPalette(kDarkBodyRadiator);
    TH2F *chi2Hist = nullptr;
    f.GetObject("chi2Final", chi2Hist);
    if (chi2Hist) {
      chi2Hist->SetTitle("Chi2/n Image");
      chi2Hist->GetXaxis()->SetTitle("X [cm]");
      chi2Hist->GetYaxis()->SetTitle("Y [cm]");
      for (size_t i=1;i<=chi2Hist->GetXaxis()->GetNbins();i++) for (size_t j=1;j<=chi2Hist->GetYaxis()->GetNbins();j++)chi2Hist->SetBinContent(i,j,chi2Hist->GetBinContent(i,j)/fNsipms);
      chi2Hist->Draw("colz");

      c2->cd();
      c2->SetLogz();
      c2->Update();
    } else {cout << "\nWARNING: Couldn't find chi2 hist...\n";}
  } else {cout << "\nWARNING: Couldn't open root file...\n";}
  ///////////////////

  ///////////////////
  // Updating canvas 3
  // Grab the bottom canvas
  TCanvas *c3 = fCanvas3->GetCanvas();
  c3->Clear();
  if (fDataType == "mc")
  {
    gStyle->SetPalette(kDarkBodyRadiator);

    if (fPrimHist) 
    {
      fPrimHist->SetTitle("True Image");
      fPrimHist->GetXaxis()->SetTitle("X [cm]");
      fPrimHist->GetYaxis()->SetTitle("Y [cm]");
      fPrimHist->Draw("colz");
      c3->cd();
      c3->Update();
    } else {cout << "\nWARNING: Couldn't find true distribution...\n";}
  }
  ////////////////////

  ////////////////////
  // Updating canvas 4
  TCanvas *c4 = fCanvas4->GetCanvas();
  c4->Clear();
  gStyle->SetOptStat(0);
  // Make a histogram of the data
  TH1I *h = new TH1I("h", "Measured Light Yield", fRecoHelper.theData.size(), 0.5, fRecoHelper.theData.size()+0.5);
  for (const auto& d : fRecoHelper.theData) h->SetBinContent(d.first, d.second);
  h->SetLineColor(4);
  h->SetLineWidth(4);
  h->GetXaxis()->SetTitle("SiPM ID");
  h->Draw();
  int max = h->GetMaximum();
  if (f.IsOpen()) {
    TH1I *expdataHist = nullptr;
    f.GetObject("expdata", expdataHist);
    if (expdataHist) {
      expdataHist->SetMarkerColor(1);
      expdataHist->SetMarkerStyle(8);
      expdataHist->Draw("p same");
      int thisMax = expdataHist->GetMaximum();
      if (thisMax > max) h->SetMaximum(thisMax);
      h->SetMinimum(0);
    } else {cout << "\nWARNING: Couldn't find expdata hist...\n";}
  }
  c4->cd();
  c4->Update();
  ///////////////////
  
}

//------------------------------------------------------------------------
void MyMainFrame::SetParameters()
{
  // Get the current settings

  // Do not allow to change disk radius
  stringstream stream;
  stream << fixed << setprecision(2) << fDiskR;
  fDiskREnt->SetText(stream.str().c_str());


  fNsipms    = std::stoi(fNsipmsEnt->GetText());
  fPixelSize = std::stod(fPixelSizeEnt->GetText());
  fGamma     = std::stod(fGammaEnt->GetText());
  std::string doPenalizedStr = doPenalizedBut->IsOn() ? "yes" : "no";
  fUnpenalizedIter = std::stoi(fUnpenalizedIterEnt->GetText());
  fPenalizedIter   = std::stoi(fPenalizedIterEnt->GetText());

  std::cout << "\n"
            << "Updating parameters...\n"
            << "Disk radius set to:        " << fDiskR     << "\n"
            << "Number of SiPMs set to:    " << fNsipms    << "\n"
            << "Pixel size set to:         " << fPixelSize << "\n"
            << "Gamma set to:              " << fGamma     << "\n"
            << "Do penalized:              " << doPenalizedStr << "\n"
            << "Unpenalized iterations:    " << fUnpenalizedIter << "\n"
            << "Penalized iterations:    " << fPenalizedIter << "\n"
            << std::endl;

  // Update reco helper
  UpdateRecoHelper();
}

//------------------------------------------------------------------------
bool MyMainFrame::IsDAQFileModified() 
{
  struct stat fileStat;
  int err = stat(fDataFilePath.c_str(), &fileStat);
  if (err != 0) {
    perror(fDataFilePath.c_str());
  }
  if (fileStat.st_mtime > fLastUpdate) {
    fLastUpdate = fileStat.st_mtime; 
    return true;
  }
  return false;
}

//------------------------------------------------------------------------
void MyMainFrame::HandleTimer() 
{
  if (!fIsRunning && IsDAQFileModified()) 
  {
    std::cout << "Detected new DAQ file!\n";
    // Grab the true distribution
    TFile s(fTrueDistPath.c_str(), "READ");
    if (s.IsOpen()) {
      s.GetObject("primHist", fPrimHist);
      if (!fPrimHist) cout << "\nWARNING: Couldn't find true distribution...\n";
      //s.Close();
    } else {cout << "\nWARNING: Couldn't find simulate output path named \'"+fTrueDistPath+"\'...\n";}
    StartReco();
  }
}

//------------------------------------------------------------------------
void EventDisplay(std::string topDir) 
{
  // Popup the GUI...
  new MyMainFrame(gClient->GetRoot(),1500,1000, topDir);
}
