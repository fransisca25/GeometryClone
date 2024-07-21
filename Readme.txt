Program Specification
h
In this assignment you will be writing the game that was presented in class.
This game must have following features:

Player:
- The player is represented by a shape which is defined in the config file
- The player must spam in the center of the screen at the beginning of the 
  game, and after it dies (collides with an enemy)
- The player moves by a speed read from the config file in these directions:
  Up: W key, Left: A key, Down: S key, Right: D key
- The player is confined to move only within the bounds of the window
- The player will shoot a bullet toward the mouse pointer when the left mouse
  button is clicked. The speed, size, and lifespan of the bullets are read 
  from the config file

Special Ability
- You are free to come up with your own 'special move' which is fired by
  the player when the right mouse button is clicked. This special ability must:
  - Multiple entities (bullets etc) spawned by special weapon
  - Entities have sime unique graphic associate with them
  - A unique game mechanic is introduced via a new component
  - A 'cooldown timer' must be implemented for the special weapon
  The properties of the special move are not in the config file.

Enemy(s):
- Enemies will spawn in a random location on the screen every X frames, 
  where X is defined in the configuration file
- Enemies must not overlap the sides of the screen at the time of spawn
- Enemies shapes have random number of vertices, between a given minimum and 
  maximum number, which is specified in the config file
- Enemy shape radius will be specified in the config file
- Enemies will be given a random color upon spawning
- Enemies will be given a random speed upon spawning, between a minimum and 
  maximum value specified in the config file
- When an enemy reaches the edge of the window, it should bounce off in
  the opposite direction at the same speed
- When (large) enemies collide with a bullet or player, they are destroyed, 
  and N small enemies spawn in its place, where N is the number of vertices 
  of the original enemy. Each small enemy must have the same number of
  vertices and color of the original enemy. These small entities travel
  outward at angles at a fixed intervals equal to (360 / vertices)
  For example, if the original enemy had 6 sides, the 6 smaller enemies will
  travel outward in the intervals of (360/6) = 60 degrees. The smaller enemies 
  must have a radius equal to half of the original entity

Score:
- Each time an enemy spawns, it is given a score component of N*100, where N
  is the number of vertices it has. Small enemies get double this value. 
- If a player bullet kills an enemy, the game score is increased by the score 
  component of the enemy killed
- The score should be displayed with the font specified bt the config file in
  the top-left corner of the screen

Drawing:
- In the render system, all entities should be given a slow rotation, which
  makes the game look a little nicer
- Any entity with a lifespan is currently alive, it should have its color
  alpha channel set ti a ratio depending on how long it has left to live. 
  For example, if an entity has 100 frame life span, and it has been alive for
  50 frames, its alpha value should be set to 0.5 * 255. The alpha should go from
  255 when it is first spawned, to 0 on the last frame it is alive.

Configuration File:
The configuration file will have one line each specifying the window size, 
font format, player, bullet specification, and enemy specifications.
Lines will be given in that order, with the following syntax:

Window W H FL FS
- This line declares that the SFML window must be constructed with width W
  and height H, each of which will be integers. FL is the frame limit that the 
  window should be set to, and FS will be an integer which specifies whether to
  display the application in full-screen mode (1) or not (0).

Font F S R G B
- This lines defines the font which is to be used to draw text
  for this program. The format od the line is as follows:
  Font file		F			std::string (it will have no spaces)
  Font size		S			int
  RGB color		(R, G, B)	int, int, int

Player Specification:
Player SR CR S FR FG OR OG OB OT V
  Shape radius			SR			int
  Collision radius		CR			int 
  Speed					S			float 
  Fill color			FR,FG,FB	int, int, int
  Outline color			OR,OG,OB	int, int, int
  Outline thickness		OT			int 
  Shape vertices		V			int

Enemy Specification:
Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI
  Shape radius			SR			int
  Collision radius		CR			int
  Min/Max speed			SMIN,SMAX	float, float
  Outline color			OR,OG,OB	int, int, int
  Outline thickness		OT			int
  Min/Max vertices		VMIN,VMAX	int, int
  Small lifespan		L			int
  Spawn interval		SP			int

Bullet Specification:
Bullet SR CR S FR FG FB OR OG OB OT V L
  Shape radius			SR			int
  Collision radius		CR			int
  Speed					S			float
  Fill color			FR,FG,FB	int, int, int
  Outline color			OR,OG,OB	int, int, int
  Outline thickness		OT			int
  Shape vertices		V			int
  Lifespan				L			int


Assignment Hints
0. Save the configuration file reading until laterm after Entities implemented
1. Implement the Vec2 class, which you will use for all components
2. Implement the basic functionality in the EntityManager class
   Implement the EntityManager's addEntity() and update() functions so
   you can start testing the Game class quickly. Don't worry about the update()
   function which deletes dead entities until later when you get more game 
   mechanics working
3. Implement basics of the Game class:
   a. Construct a player Entity using the spawnPlayer() function
   b. Implement basic drawing of entities using the Game::sRender() function
   c. Construct some enemies using the spawnEnemy() function
   d. Construct a bullet using the spawnBullet() function
4. Implement player movement in Game::userINput and Game::sMovement
5. Implement EntityManager::update() function so it deletes dead entity
6. Implement the EntityManager::getEntities(tag) functionality
7. Implement collisions in sCollision and entity.destroy() if it's dead
8. Implement the rest of the game's functionality, including config file reading