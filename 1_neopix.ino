

const uint16_t count = 166; // make sure to set this to the number of pixels in your strip
const uint8_t PixelPin = 3;  // make sure to set this to the correct pin, ignored for Esp8266
const uint8_t AnimationChannels = 1; // we only need one as all the pixels are animated at once

float maxlum = 0.05;

NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma400KbpsMethod> strip(count, PixelPin);
NeoPixelAnimator animations(AnimationChannels); // NeoPixel animation management object

void leds_off(){
  strip.ClearTo(RgbColor(0));
  strip.Show();
}

boolean fadeToColor = true;  // general purpose variable used to store effect state
struct simple_data {
  RgbColor color;
};

simple_data simple_d;

struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};

// one entry per pixel to match the animation timing manager
MyAnimationState animationState[AnimationChannels];

void SetRandomSeed()
{
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(0);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3)
    {
        seed ^= analogRead(0) << shifts;
        delay(1);
    }

    randomSeed(seed);
}

// simple blend function
void BlendAnimUpdate(const AnimationParam& param)
{
    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
    RgbColor updatedColor = RgbColor::LinearBlend(
        animationState[param.index].StartingColor,
        animationState[param.index].EndingColor,
        param.progress);

    // apply the color to the strip
    for (uint16_t pixel = 0; pixel < count; pixel++)
    {
        strip.SetPixelColor(pixel, updatedColor);
    }
}

void FadeInFadeOutRinseRepeat(float luminance)
{
    if (fadeToColor)
    {
        // Fade upto a random color
        // we use HslColor object as it allows us to easily pick a hue
        // with the same saturation and luminance so the colors picked
        // will have similiar overall brightness
        RgbColor target = HslColor(random(360) / 360.0f, 1.0f, luminance);
        uint16_t time = random(800, 2000);

        animationState[0].StartingColor = strip.GetPixelColor(0);
        animationState[0].EndingColor = target;

        animations.StartAnimation(0, time, BlendAnimUpdate);
    }
    else 
    {
        // fade to black
        uint16_t time = random(600, 700);

        animationState[0].StartingColor = strip.GetPixelColor(0);
        animationState[0].EndingColor = RgbColor(0);

        animations.StartAnimation(0, time, BlendAnimUpdate);
    }

    // toggle to the next effect state
    fadeToColor = !fadeToColor;
}

void setup_neopix()
{
    strip.Begin();
    strip.Show();

    SetRandomSeed();
}

void loop_randomblink()
{
  
    if (animations.IsAnimating())
    {
        // the normal loop just needs these two to run the active animations
        animations.UpdateAnimations();
        strip.Show();
    }
    else
    {
        // no animation runnning, start some 
        //
        FadeInFadeOutRinseRepeat(maxlum); // 0.0 = black, 0.25 is normal, 0.5 is bright
    }
}

void blinkRGB(RgbColor color, uint16_t duration, uint16_t gap) {
  strip.ClearTo(color);
  strip.Show();
  delay(duration);
  if (gap) {
    strip.ClearTo(RgbColor(0));
    strip.Show();
    delay(gap);
  }
}
void loop_simple(){
  strip.ClearTo(simple_d.color);
  strip.Show();
}
