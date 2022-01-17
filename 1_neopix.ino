

const uint16_t count = 166; // make sure to set this to the number of pixels in your strip
const uint8_t PixelPin = 3;  // make sure to set this to the correct pin, ignored for Esp8266

NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma400KbpsMethod> strip(count);
NeoPixelAnimator anim(1);
NeoPixelAnimator snake(3);

uint16_t lastPixel = 0; // track the eye position
int8_t moveDir = 1; // track the direction of movement
// uncomment one of the lines below to see the effects of
// changing the ease function on the movement animation
AnimEaseFunction moveEase = NeoEase::QuadraticInOut;
//      NeoEase::Linear;
//      NeoEase::QuadraticInOut;
//      NeoEase::CubicInOut;
//        NeoEase::QuarticInOut;
//      NeoEase::QuinticInOut;
//      NeoEase::SinusoidalInOut;
// NeoEase::ExponentialInOut;
//      NeoEase::CircularInOut;


void leds_off(){
  strip.ClearTo(RgbColor(0));
  strip.Show();
}

void FadeAll(uint8_t darkenBy)
{
    RgbColor color;
    for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++) {
        color = strip.GetPixelColor(indexPixel);
        color.Darken(darkenBy);
        strip.SetPixelColor(indexPixel, color);
    }
}
boolean fadeToColor = true;  // general purpose variable used to store effect state
struct simple_data {
  RgbColor color;
};
struct snake_data {
  RgbColor startcol;
  RgbColor endcol;
  RgbColor currentcol;
};
snake_data snake_d;
simple_data simple_d;

struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};

// one entry per pixel to match the animation timing manager
MyAnimationState animationState[1];

void SetRandomSeed()
{
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(0);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3){
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
    for (uint16_t pixel = 0; pixel < count; pixel++){
        strip.SetPixelColor(pixel, updatedColor);
    }
}

void FadeInFadeOutRinseRepeat(float luminance)
{
    if (fadeToColor){
        // Fade upto a random color
        // we use HslColor object as it allows us to easily pick a hue
        // with the same saturation and luminance so the colors picked
        // will have similiar overall brightness
        RgbColor target = HslColor(random(360) / 360.0f, 1.0f, luminance);
        uint16_t time = random(800, 2000);

        animationState[0].StartingColor = strip.GetPixelColor(0);
        animationState[0].EndingColor = target;

        anim.StartAnimation(0, time, BlendAnimUpdate);
    }else {
        // fade to black
        uint16_t time = random(600, 700);

        animationState[0].StartingColor = strip.GetPixelColor(0);
        animationState[0].EndingColor = RgbColor(0);

        anim.StartAnimation(0, time, BlendAnimUpdate);
    }

    // toggle to the next effect state
    fadeToColor = !fadeToColor;
}

void setup_neopix(){
    strip.Begin();
    strip.Show();

    SetRandomSeed();
    setup_randblink(20);
}
void FadeAnimUpdate(const AnimationParam& param)
{
    if (param.state == AnimationState_Completed)
    {
        FadeAll(10);
        snake.RestartAnimation(param.index);
    }
}

void MoveAnimUpdate(const AnimationParam& param)
{
    float progress = moveEase(param.progress);
    uint16_t nextPixel;
    if (moveDir > 0) {
        nextPixel = progress * count;
    } else {
        nextPixel = (1.0f - progress) * count;
    }

    // if progress moves fast enough, we may move more than
    // one pixel, so we update all between the calculated and
    // the last
    if (lastPixel != nextPixel) {
        for (uint16_t i = lastPixel + moveDir; i != nextPixel; i += moveDir) {
            strip.SetPixelColor(i, snake_d.currentcol);
        }
    }
    strip.SetPixelColor(nextPixel, snake_d.currentcol);

    lastPixel = nextPixel;

    if (param.state == AnimationState_Completed) {
        // reverse direction of movement
        moveDir *= -1;

        // done, time to restart this position tracking animation/timer
        snake.RestartAnimation(param.index);
    }
}

bool forwardDirBlend = true;

void BlendColorUpdate(const AnimationParam& param) {
    if (param.state == AnimationState_Completed) {
      snake.RestartAnimation(param.index);
      forwardDirBlend = !forwardDirBlend;
    }

    float progress = NeoEase::CubicInOut(param.progress);
    if (forwardDirBlend) {
      snake_d.currentcol = RgbColor::LinearBlend(snake_d.startcol, snake_d.endcol, progress);
    } else {
      snake_d.currentcol = RgbColor::LinearBlend(snake_d.endcol, snake_d.startcol, progress);
    }
}
float maxlum = 0.05;
void setup_randblink(int lum){
  maxlum = mapfloat(lum, 0,100,0,0.5);
}

void loop_randblink(){
    if (anim.IsAnimating()) {
        // the normal loop just needs these two to run the active animations
        anim.UpdateAnimations();
        strip.Show();
    }else {
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

//-------------------------------------------------------------------
void setup_snake(){
  // fade all pixels providing a tail that is longer the faster the pixel moves.
    snake.StartAnimation(0, 7, FadeAnimUpdate);

    // take several seconds to move eye fron one side to the other
    snake.StartAnimation(1, 1000, MoveAnimUpdate);
    // and linear blending of the color over the duration
    snake.StartAnimation(2, 1000, BlendColorUpdate);
}
void loop_snake(){
  snake.UpdateAnimations();
  strip.Show();
}
