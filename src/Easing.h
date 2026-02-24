#pragma once

namespace Easing {

    
    static constexpr float c1 = 1.70158f;
    static constexpr float c2 = c1 * 1.525f;
    static constexpr float c3 = c1 + 1.0f;
    static constexpr float pi = 3.14159265358979323846f;
    static constexpr float c4 = (2.0f * pi) / 3.0f;
    static constexpr float c5 = (2.0f * pi) / 4.5f;

    static float bounceOutInternal(float x) {
        const float n1 = 7.5625f;
        const float d1 = 2.75f;

        if (x < 1.0f / d1) return n1 * x * x;
        if (x < 2.0f / d1) return n1 * (x -= 1.5f / d1) * x + 0.75f;
        if (x < 2.5f / d1) return n1 * (x -= 2.25f / d1) * x + 0.9375f;
        return n1 * (x -= 2.625f / d1) * x + 0.984375f;
    }

    float linear(float x) { return x; }

    float easeInQuad(float x) { return x * x; }
    float easeOutQuad(float x) { return 1 - (1 - x) * (1 - x); }
    float easeInOutQuad(float x) { return x < 0.5f ? 2 * x * x : 1 - std::pow(-2 * x + 2, 2) / 2; }

    float easeInCubic(float x) { return x * x * x; }
    float easeOutCubic(float x) { return 1 - std::pow(1 - x, 3); }
    float easeInOutCubic(float x) { return x < 0.5f ? 4 * x * x * x : 1 - std::pow(-2 * x + 2, 3) / 2; }

    float easeInQuart(float x) { return x * x * x * x; }
    float easeOutQuart(float x) { return 1 - std::pow(1 - x, 4); }
    float easeInOutQuart(float x) { return x < 0.5f ? 8 * x * x * x * x : 1 - std::pow(-2 * x + 2, 4) / 2; }

    float easeInQuint(float x) { return x * x * x * x * x; }
    float easeOutQuint(float x) { return 1 - std::pow(1 - x, 5); }
    float easeInOutQuint(float x) { return x < 0.5f ? 16 * x * x * x * x * x : 1 - std::pow(-2 * x + 2, 5) / 2; }

    float easeInSine(float x) { return 1 - std::cos((x * pi) / 2); }
    float easeOutSine(float x) { return std::sin((x * pi) / 2); }
    float easeInOutSine(float x) { return -(std::cos(pi * x) - 1) / 2; }

    float easeInExpo(float x) { return x == 0 ? 0 : std::pow(2, 10 * x - 10); }
    float easeOutExpo(float x) { return x == 1 ? 1 : 1 - std::pow(2, -10 * x); }
    float easeInOutExpo(float x) {
        if (x == 0) return 0;
        if (x == 1) return 1;
        if (x < 0.5f) return std::pow(2, 20 * x - 10) / 2;
        return (2 - std::pow(2, -20 * x + 10)) / 2;
    }

    float easeInCirc(float x) { return 1 - std::sqrt(1 - x * x); }
    float easeOutCirc(float x) { return std::sqrt(1 - std::pow(x - 1, 2)); }
    float easeInOutCirc(float x) {
        if (x < 0.5f) return (1 - std::sqrt(1 - std::pow(2 * x, 2))) / 2;
        return (std::sqrt(1 - std::pow(-2 * x + 2, 2)) + 1) / 2;
    }

    float easeInBack(float x) { return c3 * x * x * x - c1 * x * x; }
    float easeOutBack(float x) { return 1 + c3 * std::pow(x - 1, 3) + c1 * std::pow(x - 1, 2); }
    float easeInOutBack(float x) {
        if (x < 0.5f)
            return (std::pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2;
        return (std::pow(2 * x - 2, 2) * ((c2 + 1) * (2 * x - 2) + c2) + 2) / 2;
    }

    float easeInElastic(float x) {
        if (x == 0) return 0;
        if (x == 1) return 1;
        return -std::pow(2, 10 * x - 10) * std::sin((x * 10 - 10.75f) * c4);
    }

    float easeOutElastic(float x) {
        if (x == 0) return 0;
        if (x == 1) return 1;
        return std::pow(2, -10 * x) * std::sin((x * 10 - 0.75f) * c4) + 1;
    }

    float easeInOutElastic(float x) {
        if (x == 0) return 0;
        if (x == 1) return 1;
        if (x < 0.5f)
            return -(std::pow(2, 20 * x - 10) * std::sin((20 * x - 11.125f) * c5)) / 2;
        return (std::pow(2, -20 * x + 10) * std::sin((20 * x - 11.125f) * c5)) / 2 + 1;
    }

    float easeInBounce(float x) { return 1 - bounceOutInternal(1 - x); }
    float easeOutBounce(float x) { return bounceOutInternal(x); }
    float easeInOutBounce(float x) {
        if (x < 0.5f)
            return (1 - bounceOutInternal(1 - 2 * x)) / 2;
        return (1 + bounceOutInternal(2 * x - 1)) / 2;
    }

}