# “Man in Black,” a top-down 2D shooter where your mission is simple:

Kill all the aliens before they get to you.

Let’s begin with the Menu Screen.

The game opens with a minimal black screen that says:

Man in Black

Kill all the Aliens

Press Enter to Start

It’s clean, dramatic, and handled as a proper separate Scene class—not just text on the screen.

Gameplay Overview.

When you press Enter, the action starts immediately.

The player moves freely in four directions using the arrow keys.

You can shoot laser bullets using the spacebar—but only left or right, not up or down.

This was an intentional design choice. In a top-down shooter, shooting up and down felt awkward and unrealistic—so we restricted shooting to horizontal only.

It adds challenge and makes your position matter more.

Levels and Duration.

There are three levels, each using different tile maps, and each one slightly harder in terms of layout and number of enemies.

But gameplay remains consistent throughout, and the full experience easily lasts over 2 minutes.

Enemies and AI.

The aliens—our main AI enemies—move side to side, from left to right and back again.

Every alien in all three levels uses this same movement pattern, but in different numbers and arrangements.

They’re simple, but deadly—if even one touches the player, you lose immediately.

Win/Lose Conditions.

You lose if an alien touches you.
You win by making it to Level 3 and defeating all enemies.

When you die, a message says “You Died” on screen.

If you win, it says “Great Job!”

Shader Logic.

We implemented a logic-based shader effect that uses an if statement to darken the screen when the player dies.

It connects visually to the emotional tone of the game and contributes to the atmosphere.

Audio.

The game includes:

1 background music track that loops

3 custom sound effects: walking, shooting, and death

All of these were new assets, not reused from any lecture or previous assignment.

Conclusion.

Man in Black was built entirely within the structure of the assignment:

We used delta time, fixed time step, and the Entity/Map/Scene system.

All assets are original.

We implemented sound, AI, win/loss logic, and shader effects.

Thank you for listening—and good luck staying alive.
