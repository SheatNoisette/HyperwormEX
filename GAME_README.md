
        ▗▖ ▗▖▗▖  ▗▖▗▄▄▖ ▗▄▄▄▖▗▄▄▖ ▗▖ ▗▖ ▗▄▖ ▗▄▄▖ ▗▖  ▗▖    ▗▄▄▄▖▗▖  ▗▖
        ▐▌ ▐▌ ▝▚▞▘ ▐▌ ▐▌▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▛▚▞▜▌    ▐▌    ▝▚▞▘
        ▐▛▀▜▌  ▐▌  ▐▛▀▘ ▐▛▀▀▘▐▛▀▚▖▐▌ ▐▌▐▌ ▐▌▐▛▀▚▖▐▌  ▐▌    ▐▛▀▀▘  ▐▌
        ▐▌ ▐▌  ▐▌  ▐▌   ▐▙▄▄▖▐▌ ▐▌▐▙█▟▌▝▚▄▞▘▐▌ ▐▌▐▌  ▐▌    ▐▙▄▄▖▗▞▘▝▚▖

                   A game by HeraldOD & SheatNoisette
                        4MB 2025 GAME JAM ENTRY

In 2142, an interstellar Megacorp Tapax Industries discovered Pentieum-i7, a
rare energy source buried beneath the toxic surface of 4MB-4269, a desolate
exoplanet. The catch? The mineral is guarded by Sillycrawlers, armored,
worm-like creatures that defend their territory.

You play as Herald Shnt, a debt-trapped contractor sent to 4MB-4269 under
Tapax Industries’s "Drill First, Ask Never" policy.

Your job:
* EXTERMINATE Sillycrawlers (each kill deducts from your crippling debt!).
* DESTROY terrain with plasma charges to reach Pentieum-i7 veins.
* UNCOVER a mysterious energy signature deep underground rumored to be a stable
  wormhole leading to layers of dimensions where Sillycrawlers progressively
  evolved into godlike beings.

Tapax doesn’t care about the portal... Unless it’s profitable... Blow up enough
worms, and you might just buy your freedom, or die in a hail of molten chitin.

~ Good luck Herald, see you at the job. As always, farwell.

## HOW TO RUN

After extracting the archive, open a terminal in the same folder as the game.
Then, type:
```
$ chmod +x ./HYPERWORM-EX
$ ./HYPERWORM-EX
```

## HOW TO PLAY

* WASD / ZQSD / Arrow keys: Move
* Mouse: Look around
* Left click: Shoot !
* Right click: Launch a grenade !
* Arrows / Enter: Navigate menus
* O / L: Increase or decrease mouse sensitivity

## TRIVIA

Every game has its secrets, here's some juicy ones:

- The dithering method is the same technique used in Unreal-1's (1998) software
  renderer.
- The first version of the game engine was a Wolfenstein 3d style raycaster
  with basic lightmaps. The idea was pushed to the extreme and a DDA
  raycasting algorithm to simulate a 3d world was used instead (Like
  Minecraft4k).
- The game uses a software renderer because we didn’t have the time or the will
  to learn proper OpenGL. A basic GL Context is still used as a glorified
  framebuffer rescaler. Zero shaders used.
- The game contains a feature packed custom game engine. Featuring an entity
  system, collisions, lights, renderer and sprite rotation/drawing.
- The initial idea for the game was a clone of Hotline Miami or a spaceship
  exploration game.
- The game engine was originally not going to be used in the jam
- The name of the engine is "PotetreVoxels", which is the direct successor of
  Potetre2d.
- The game is written 99% in C with 4k LoC

                 Stay tunned for the source code release
                      Gameplay    -       HeraldOD
                       Engine     -     SheatNoisette
## LICENSES

This game use a modified version of PL Synth by Dominic Szablewski:
```
Copyright (c) 2024, Dominic Szablewski - https://phoboslab.org
SPDX-License-Identifier: MIT

Based on Sonant, published under the Creative Commons Public License
(c) 2008-2009 Jake Taylor [ Ferris / Youth Uprising ]
```