/**
 * @file DayCycle.h
 * @brief Declares the DayCycle class for time-of-day progression.
 *
 * @author Group 46
 */
#pragma once
#include <string>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

/**
 * @enum TimeOfDay
 * @brief The four named phases of the day cycle.
 */
enum class TimeOfDay {
    Morning, ///< Sunrise — orange/pink sky, sun rises from left.
    Noon,    ///< Full daylight — bright blue sky, sun overhead.
    Evening, ///< Sunset — golden/orange sky fading to dark.
    Night    ///< Darkness — navy sky, stars, moon, faster sleep drain.
};

/**
 * @class DayCycle
 * @brief Tracks the passage of in-game time and exposes sky/celestial visuals.
 *
 * One full day = DAY_DURATION real seconds (default 600 = 10 minutes).
 * The normalised dayTime value [0, 1) is divided into four equal phases:
 *
 * | Range        | Phase   | Notes                              |
 * |:-------------|:--------|:-----------------------------------|
 * | 0.00 – 0.25  | Morning | Sunrise, sleep drains normally      |
 * | 0.25 – 0.50  | Noon    | Bright blue, sun at peak            |
 * | 0.50 – 0.75  | Evening | Sunset fading to dark               |
 * | 0.75 – 1.00  | Night   | Dark, stars, moon, 2× sleep drain  |
 *
 * Sky colour, sun/moon position, and night darkness are all computed
 * analytically from dayTime so there are no sudden transitions.
 *
 * @author Group 46
 */
class DayCycle {
public:
    static constexpr float DAY_DURATION   = 600.f; ///< Real seconds per full day.
    static constexpr float MORNING_START  = 0.00f; ///< dayTime at start of Morning.
    static constexpr float NOON_START     = 0.25f; ///< dayTime at start of Noon.
    static constexpr float EVENING_START  = 0.50f; ///< dayTime at start of Evening.
    static constexpr float NIGHT_START    = 0.75f; ///< dayTime at start of Night.

    /**
     * @brief Advances the day cycle by dt real seconds.
     *
     * Wraps dayTime back to 0 and increments dayCount when a full day elapses.
     *
     * @param dt Delta time in seconds from the main game loop.
     */
    void update(float dt);

    /**
     * @brief Skips time forward to the start of the next morning.
     *
     * Called when the player sleeps in a Bed during Night. Increments
     * dayCount if currently in Night phase.
     */
    void skipToMorning();

    /**
     * @brief Returns the current phase of the day.
     * @return TimeOfDay enum value for the current dayTime.
     */
    TimeOfDay   getPhase()     const;

    /**
     * @brief Returns true during Night phase (dayTime >= NIGHT_START).
     * @return True if it is currently Night.
     */
    bool        isNight()      const;

    /**
     * @brief Returns true during Morning phase (dayTime < NOON_START).
     * @return True if it is currently Morning.
     */
    bool        isMorning()    const;

    /**
     * @brief Returns the normalised day progress in [0, 1).
     * @return Current position within the day cycle.
     */
    float       getDayTime()   const;

    /**
     * @brief Returns the total number of days elapsed since game start.
     * @return Day count starting at 1.
     */
    int         getDayCount()  const;
    void        setDayTime(float t);
    void        setDayCount(int c);

    /**
     * @brief Returns the human-readable name of the current phase.
     * @return One of "Morning", "Noon", "Evening", "Night".
     */
    std::string getPhaseName() const;

    /**
     * @brief Returns the sky background colour for the current dayTime.
     *
     * Smoothly interpolates between key colours at phase boundaries so
     * there are no sudden jumps — the transition is continuous each frame.
     *
     * @return SFML Color for the sky fill.
     */
    sf::Color   getSkyColor()  const;

    /**
     * @brief Returns the sun's screen position during daylight hours.
     *
     * The sun travels from left to right along a sine arc. Returns {-1,-1}
     * during Night phase (sun is below the horizon).
     *
     * @param screenW Window width in pixels.
     * @param screenH Window height in pixels.
     * @return Screen-space position of the sun centre.
     */
    sf::Vector2f getSunPosition(float screenW, float screenH)  const;

    /**
     * @brief Returns the moon's screen position during Night phase.
     *
     * Returns {-1,-1} during daytime.
     *
     * @param screenW Window width in pixels.
     * @param screenH Window height in pixels.
     * @return Screen-space position of the moon centre.
     */
    sf::Vector2f getMoonPosition(float screenW, float screenH) const;

    /**
     * @brief Returns a darkness multiplier [0, 1] for the night overlay.
     *
     * 0 = fully bright (day), 1 = maximum darkness (deep night).
     * Ramps up gradually at dusk and fades at dawn.
     *
     * @return Opacity fraction for the dark blue overlay drawn over the world.
     */
    float        getNightDarkness() const;

private:
    float dayTime  = 0.15f; ///< Normalised position within the day [0, 1).
    int   dayCount = 1;     ///< Number of days elapsed; starts at 1.

    /**
     * @brief Linearly interpolates between two SFML Colors.
     * @param a Start colour.
     * @param b End colour.
     * @param t Blend factor clamped to [0, 1].
     * @return Interpolated colour.
     */
    static sf::Color lerpColor(sf::Color a, sf::Color b, float t);
};
