#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include "IPlugPaths.h"
#include "IGraphicsStructs.h"
#include "Smoothers.h"

const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kButtL = 1,
  kButtR = 2,
  kButtMS = 3,
  kButtPus = 4,
  kButtBypass = 5,
  S_On = 6,
  S_Off = 7,
  switchModes,
  vuMeters,
  kCalibx,
  kNumParams
};

enum EControlTags
{
  kCtrlTagDialogResult = 0,
  kCtrlTagVectorButton,
  kCtrlTagVectorSliderV,
  kCtrlTagVectorSliderH,
  kCtrlTagTabSwitch,
  kCtrlTagRadioButton,
  kCtrlTagScope,
  kCtrlTagDisplay,
  kCtrlTagMeter,
  kCtrlTagMeterR,
  kCtrlTagMeterM,
  kCtrlTagMeterS,
  kCtrlTagRTText,
  kCtrlTagRedLED,
  kCtrlTagGreenLED,
  kCtrlTagBlueLED,
  kCtrlTags
};


using namespace iplug;
using namespace igraphics;

class BRM1Monitor final : public Plugin
{
public:
  BRM1Monitor(const InstanceInfo& info);

#if IPLUG_EDITOR
  
#endif

#if IPLUG_DSP // http://bit.ly/2S64BDd
public:
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override {
    const double gain = GetParam(kGain)->Value() / 100;
    const int nChans = NOutChansConnected();

    double Bu_A = GetParam(S_On)->Value();
    double Bu_B = GetParam(S_Off)->Value();
    double Bu_L = GetParam(kButtL)->Value();
    double Bu_R = GetParam(kButtR)->Value();
    double Bu_Pus = GetParam(kButtPus)->Value();
    double Bu_MS = GetParam(kButtMS)->Value();
    double Bu_Bypass = GetParam(kButtBypass)->Value();
    double Bu_Switch = GetParam(switchModes)->Value();
    int k_Calib = GetParam(kCalibx)->Value();

    double cOutL = gain;
    double cOutR = gain;

    static const double DB_MINX = DBToAmp(-60.);
    static const double DB_MAXX = DBToAmp(3.);
    static const double DB_ZERO = DBToAmp(0.);
    double CalMax = 0;

    switch (k_Calib)
    {
      case 0:
        CalMax = 1;
        break;
      case 1:
        CalMax = (DBToAmp(1));
        break;
      case 2:
        CalMax = (DBToAmp(2));
        break;
      case 3:
        CalMax = (DBToAmp(3));
        break;
      case 4:
        CalMax = (DBToAmp(4));
        break;
      case 5:
        CalMax = (DBToAmp(5));
        break;
      case 6:
        CalMax = (DBToAmp(6));
        break;
      case 7:
        CalMax = (DBToAmp(7));
        break;
      case 8:
        CalMax = (DBToAmp(8));
        break;
      case 9:
        CalMax = (DBToAmp(9));
        break;
      case 10:
        CalMax = (DBToAmp(10));
        break;
      case 11:
        CalMax = (DBToAmp(11));
        break;
      case 12:
        CalMax = (DBToAmp(12));
        break;
      case 13:
        CalMax = (DBToAmp(13));
        break;
      case 14:
        CalMax = (DBToAmp(14));
        break;
      case 15:
        CalMax = (DBToAmp(15));
        break;
      case 16:
        CalMax = (DBToAmp(16));
        break;
      case 17:
        CalMax = (DBToAmp(17));
        break;
      case 18:
        CalMax = (DBToAmp(18));
        break;
    }

    if (Bu_Bypass == 1) {
      if (Bu_B == 1) {
        if (Bu_L == 1 && Bu_R == 1) {
          cOutL = gain * 0.00001;
          cOutR = gain * 0.00001;
        }
        else if (Bu_L == 1 && Bu_R == 0) {
          cOutL = gain * 0.00001;
          cOutR = gain * 0.25;
        }
        else if (Bu_L == 0 && Bu_R == 1) {
          cOutL = gain * 0.25;
          cOutR = gain * 0.00001;
        }
        else {
          cOutL = gain * 0.25;
          cOutR = gain * 0.25;
        }
      }
      else if (Bu_A == 1) {
        if (Bu_L == 1 && Bu_R == 1) {
          cOutL = gain * 0.00001;
          cOutR = gain * 0.00001;
        }
        else if (Bu_L == 1 && Bu_R == 0) {
          cOutL = gain * 0.00001;
          cOutR = gain * 0.5;
        }
        else if (Bu_L == 0 && Bu_R == 1) {
          cOutL = gain * 0.5;
          cOutR = gain * 0.00001;
        }
        else {
          cOutL = gain * 0.5;
          cOutR = gain * 0.5;
        }
      }
      else if (Bu_A == 0 && Bu_B == 0) {
        if (Bu_L == 1 && Bu_R == 1) {
          cOutL = gain * 0.00001;
          cOutR = gain * 0.00001;
        }
        else if (Bu_L == 1 && Bu_R == 0) {
          cOutL = gain * 0.00001;
          cOutR = gain;
        }
        else if (Bu_L == 0 && Bu_R == 1) {
          cOutL = gain;
          cOutR = gain * 0.00001;
        }
        else {
          cOutL = gain;
          cOutR = gain;
        }
      }
    }

    for (int s = 0; s < nFrames; s++) {

      // Output ////////////

      double AudioInL = inputs[0][s]  * mGainSmoother1.Process(cOutL);
      double AudioInR = inputs[1][s]  * mGainSmoother2.Process(cOutR);

      // Stereo
      outputs[0][s] = AudioInL;
      outputs[1][s] = AudioInR;

      if (Bu_Bypass == 1 && Bu_Pus == 1 && Bu_MS == 0) { // Mid
        outputs[0][s] = (AudioInL + AudioInR) / 2;
        outputs[1][s] = (AudioInR + AudioInL) / 2;
      }
      else if (Bu_Bypass == 1 && Bu_Pus == 1 && Bu_MS == 1) { // Side
        outputs[0][s] = (AudioInL - AudioInR) / 2;
        outputs[1][s] = (AudioInR - AudioInL) / 2;
      }

      double LeftDB = outputs[0][s];
      double RightDB = outputs[1][s];
      double DBAjust = 1.25 * CalMax;
      double sendGain = (std::fabs(LeftDB) * DBAjust);
      double sendGainR = (std::fabs(RightDB) * DBAjust);
      //double sendSide = (std::fabs((LeftDB - RightDB)/2) * DBAjust);
      double CVol = std::min(std::max(sendGain, DB_MINX), DB_MAXX);
      double CVolR = std::min(std::max(sendGainR, DB_MINX), DB_MAXX);
      //double CVolS = std::min(std::max(sendSide, DB_MINX), DB_MAXX);

      double valuexx = 0;
      double valuexxR = 0;
      //double valuexxS = 0;

      if (Bu_Bypass == 1) {
        valuexx = mPSmoother3.Process(roundf(CVol * 100 + 0.5) / 100);
        valuexxR = mPSmoother4.Process(roundf(CVolR * 100 + 0.5) / 100);
        //valuexxS = mPSmootherS.Process(CVolS);
        mParamChangeFromProcessor.Push(ParamTuple{ kCtrlTagMeter, valuexx });
        mParamChangeFromProcessor.Push(ParamTuple{ kCtrlTagMeterR, valuexxR });
        //mParamChangeFromProcessor.Push(ParamTuple{ kCtrlTagMeterS, valuexxS });
      }
      else {
        valuexx = GetParam(kCtrlTagMeter)->FromNormalized(0);
        valuexxR = GetParam(kCtrlTagMeterR)->FromNormalized(0);
        //valuexxS = GetParam(kCtrlTagMeterS)->FromNormalized(0);
        mParamChangeFromProcessor.Push(ParamTuple{ kCtrlTagMeter, (1 * mPSmoother3.Process(valuexx)) });
        mParamChangeFromProcessor.Push(ParamTuple{ kCtrlTagMeterR, (1 * mPSmoother4.Process(valuexxR)) });
        //mParamChangeFromProcessor.Push(ParamTuple{ kCtrlTagMeterS, (1 * mPSmootherS.Process(valuexxS)) });
      }
    }
  }
  void OnIdle() override;
  void OnParamChangeUI(int paramIdx, EParamSource source) override;
private:
  //IPeakSender<2> mMeterSender;
  LogParamSmooth<sample> mGainSmoother1{ 30.f };
  LogParamSmooth<sample> mGainSmoother2{ 30.f };
  LogParamSmooth<sample> mPSmoother3{ 300.f };
  LogParamSmooth<sample> mPSmoother4{ 300.f };
  //LogParamSmooth<sample> mPSmootherS{ 300.f };
#endif
};

