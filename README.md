# CMPUT 350 HW 2

## AI Usage

### Dash logic in `engine/InputSystem.h` (Claude, Anthropic)
**Prompt (Damon):** "Here are some notes on the dash, would you be able to fill in my dash logic here? Keep it simple and similar to the structure of the code i have already done." Attached were the lecture notes on the dash: a timer on how often you can dash, dash only left/right, dash outward when on a wall, and still dash into the wall when on both the floor and a wall.

**What was generated:** the block between the `// --- Dash (AI-generated ...` and `// --- end AI-generated dash ---` comments, the `!dashing &&` checks added to the speed clamp and friction, and the `kDashInterval` (changed from 25 to 60 so the dash is once per second, as the spec says) and `kDashFrames` constants.

**Reflection:** TODO (Damon)

### Ceiling, wall and fallback collision checks in `engine/CollisionSystem.h` (Claude, Anthropic)
**Prompt (Damon):** "since i have completed the ground check, fill in the other checks for me in my same style while keeping things simple and easy to follow"

**What was generated:** the `onCeiling`, `onRightWall` and `onLeftWall` checks after Damon's ground check, modelled on it, and the fallback `else` that pushes the entity out along the smaller overlap when it was already overlapping a tile last frame. The code is marked with an `// AI assistance (Claude) ...` comment. Damon later added the star bounce (`kStarBounce`) to these branches himself.

**Reflection:** TODO (Damon)

### Dynamic vs dynamic collision checks in `engine/CollisionSystem.h` (Claude, Anthropic)
**Prompt (Damon):** "since I already processed collisions for the dynamic vs static entities, fill in the dynamic vs dynamic ones for me using my same style. It shouldnt vary too much probably."

**What was generated:** inside Damon's dynamic vs dynamic loop (after his `overlapBox` setup): the real-collision check, the star collect (add to score, play `"starCollect"`, remove the star), and the star vs star bounce (swap velocities scaled by `kStarBounce`, push apart along the smaller overlap). Also the `IsValid(id)` check before the landing sound, which stops the code from using an entity that was just collected. Marked with an `// AI assistance (Claude) ...` comment.

**Reflection:** TODO (Damon)

### Screen shake in the ceiling and wall checks in `engine/CollisionSystem.h` (Claude, Anthropic)
**Prompt (Damon):** "ok now that ive done it for the ground check, copy my logic to the other 3"

**What was generated:** copies of Damon's ground-hit screen shake block in the ceiling, right wall and left wall checks, changed to use `lastFrameCeiling`/`lastFrameRightWall`/`lastFrameLeftWall` and the matching velocity axis (y for the ceiling, x for the walls).

**Reflection:** TODO (Damon)

