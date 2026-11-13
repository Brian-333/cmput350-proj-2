# CMPUT 350 HW 2

## AI Usage

### Dash logic in `engine/InputSystem.h` (Claude, Anthropic)
**Prompt (Damon):** "Here are some notes on the dash, would you be able to fill in my dash logic here? Keep it simple and similar to the structure of the code i have already done." Attached were the lecture notes on the dash: a timer on how often you can dash, dash only left/right, dash outward when on a wall, and still dash into the wall when on both the floor and a wall.

**What was generated:** the block between the `// --- Dash (AI-generated ...` and `// --- end AI-generated dash ---` comments, the `!dashing &&` checks added to the speed clamp and friction, and the `kDashInterval` (changed from 25 to 60 so the dash is once per second, as the spec says) and `kDashFrames` constants.

**Reflection:** TODO (Damon)

