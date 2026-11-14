# CMPUT 350 HW 2

## AI Usage

### Dash logic in `engine/InputSystem.h` (Claude, Anthropic)
**Prompt (Damon):** "Here are some notes on the dash, would you be able to fill in my dash logic here? Keep it simple and similar to the structure of the code i have already done." Attached were the lecture notes on the dash: a timer on how often you can dash, dash only left/right, dash outward when on a wall, and still dash into the wall when on both the floor and a wall.

**What was generated:** the block between the `// --- Dash (AI-generated ...` and `// --- end AI-generated dash ---` comments, the `!dashing &&` checks added to the speed clamp and friction, and the `kDashInterval` (changed from 25 to 60 so the dash is once per second, as the spec says) and `kDashFrames` constants.


### Ceiling, wall and fallback collision checks in `engine/CollisionSystem.h` (Claude, Anthropic)
**Prompt (Damon):** "since i have completed the ground check, fill in the other checks for me in my same style while keeping things simple and easy to follow"

**What was generated:** the `onCeiling`, `onRightWall` and `onLeftWall` checks after Damon's ground check, modelled on it, and the fallback `else` that pushes the entity out along the smaller overlap when it was already overlapping a tile last frame. The code is marked with an `// AI assistance (Claude) ...` comment. Damon later added the star bounce (`kStarBounce`) to these branches himself.


### Dynamic vs dynamic collision checks in `engine/CollisionSystem.h` (Claude, Anthropic)
**Prompt (Damon):** "since I already processed collisions for the dynamic vs static entities, fill in the dynamic vs dynamic ones for me using my same style. It shouldnt vary too much probably."

**What was generated:** inside Damon's dynamic vs dynamic loop (after his `overlapBox` setup): the real-collision check, the star collect (add to score, play `"starCollect"`, remove the star), and the star vs star bounce (swap velocities scaled by `kStarBounce`, push apart along the smaller overlap). Also the `IsValid(id)` check before the landing sound, which stops the code from using an entity that was just collected. Marked with an `// AI assistance (Claude) ...` comment.


### Screen shake in the ceiling and wall checks in `engine/CollisionSystem.h` (Claude, Anthropic)
**Prompt (Damon):** "ok now that ive done it for the ground check, copy my logic to the other 3"

**What was generated:** copies of Damon's ground-hit screen shake block in the ceiling, right wall and left wall checks, changed to use `lastFrameCeiling`/`lastFrameRightWall`/`lastFrameLeftWall` and the matching velocity axis (y for the ceiling, x for the walls).


### Gravity component construction fix in `engine/SpawnSystem.h` (Claude, Anthropic)
**Prompt (Damon):** "try to build it for me and look, also resolve the syntax"

**What was generated:** a one-line change to how spawned stars get their gravity: `GravityComponent{{0.f, 0.5f}}` became `GravityComponent(Point2D(0.f, 0.5f))`. The brace version was ambiguous to Apple clang (it could also be read as building a `GravityComponent` from two numbers).

### Wall slide in `engine/GravitySystem.h` (Claude, Anthropic)
**Prompt (Damon):** "since the checks are similar, can you fill in the wall slide for me using the style that i went with for the wall jump? youll have to get the collision component too i am guessing"

**What was generated:** the wall slide block after the gravity is applied (marked with an `// AI assistance (Claude) ...` comment), the `kWallSlideSpeed` constant and the `CollisionComponent.h` include. When the player is falling, not on the ground and holding into a wall, the fall speed is capped at `kWallSlideSpeed` and the `"wallSlide"` sound plays, unless it is already playing.


### Fixes for jumping next to walls and the wall slide sound (Claude, Anthropic)
**Prompt (Damon):** "The wall slide sound plays repeatedly while sliding down a wall. theres also a bug that wont let the player jump if they are on the ground and holding towards the wall. can you fix these"

**What was generated:**
- `engine/CollisionSystem.h`: the side detection before the ground check (marked with an `// AI assistance (Claude) ...` comment). The four checks now use `wasAbove`/`wasBelow`/`wasLeft`/`wasRight`, and when the entity came at a tile diagonally it is resolved along the axis with the smaller overlap. The cause of the jump bug: jumping while pushing into a wall moves the player up beside the wall tile above them, and because that tile was above the player last frame, the old checks treated it as a ceiling and cancelled the jump.
- `engine/GravitySystem.h` and `engine/LocationComponent.h`: a `wallSliding` flag on `GravityComponent`, so the `"wallSlide"` sound plays once when a slide starts instead of repeating while sliding.


### Value tuning (Claude, Anthropic)
**Prompt (Damon):** "Ok we are all done. For some values I would like you to tune: Camera shake is way too strong right now. Player should be double its size. Jump height is WAY too high. Stars spawn too fast. The dash is too fast. Player is too fast. For these small tunings, you dont need to reference ai usage actually in the code since they are just values we are changing but still add this prompt in the readme"

**What was changed (values only):**
- `engine/CollisionSystem.h`: `kShakeScale` 0.5 to 0.15, `kShakeFrames` 8 to 6.
- `engine/InputSystem.h`: `kJumpVelocity` 25 to 13, `kMaxRunVelocity` 15 to 7, `kGroundRunAcceleration` 5 to 1, `kAirRunAcceleration` 4 to 0.7, `kDashVelocity` 50 to 18, `kWallJumpVelocity` 10 to 7 (kept equal to the max run speed so the speed clamp doesn't cut wall jumps short).
- `platformer/main.cpp`: player sprite drawn at 96x96 instead of 64x64 (1.5x), collision box scaled to match as `{{18, 20}, 61, 76}`; star spawner first spawn after 60 frames, then every 120 frames instead of every 30.

### Reflection (Damon)
I found AI to be very useful for the project to do some of the tedious work. I worked on a lot of the game implementation and systems for dealing with jumping and collisions. For a lot of these systems they followed the pattern of doing 1 check and then doing a similar check 3 more times with very slightly different logic. I was able to do the first check on my own in order to learn how the systems work and then quickly get AI to fill in the rest, while not missing out on much learning. It was also useful for debugging at the very end (although we didn't have too much to debug). For quality, I had it mimic my code style so that I would be able to understand it well. It did not write anything which I was not able to decipher. It was also very successful for debugging at the end.
