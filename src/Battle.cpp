/* Battle.cpp
 *
 * Copyright 2005-2007 Eliot Eshelman
 * battlestartux@6by9.net
 *
 *
 *  This file is part of Battlestar Tux.
 *
 *  Battlestar Tux is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  Battlestar Tux is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Battlestar Tux; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include <math.h>

#include "Battle.h"
#include "Config.h"
#include "EnemyFighter.h"
#include "FighterAmmo.h"
#include "TextureManager.h"
#include "Vector.h"

Battle::Battle( Game* g ) {
	game = g;

	aborted = false;
	finished = false;

	screen = game->getScreen();

	keyLeft = false;
	keyRight = false;
	keyUp = false;
	keyDown = false;

	hud = new HUD( game );
	ground = new Ground( SOLID_GROUND, game );

	hero = new HeroFighter( game );
	game->setFighter( hero );

	heroAmmoList = new FighterAmmoList( game );
	game->setHeroAmmoList( heroAmmoList );

	enemies = new EnemyFighterList( game );
	game->setEnemyFighterList( enemies );

	explosionList = new ExplosionList( game );
	game->setExplosionList( explosionList );

	enemyAmmoList = new FighterAmmoList( game );
	game->setEnemyAmmoList( enemyAmmoList );
}


Battle::~Battle() {
	delete hud;
	delete ground;
	delete hero;
	delete heroAmmoList;
	delete enemies;
	delete explosionList;
	delete enemyAmmoList;
}


void Battle::Update() {
	SDL_Event event;

	// If not paused, update all positions/states.
	if( !game->isPaused() ) {
		heroAmmoList->UpdateObjects();
		enemyAmmoList->UpdateObjects();

		heroAmmoList->CheckCollisions( enemies );
		enemyAmmoList->CheckCollisions( hero );

		explosionList->Update();

		hero->Update();
		if( hero->getHealth() <= 0 )
			FinishBattle();

		enemies->UpdateObjects();

		enemies->CheckCollisions( hero );

		ground->Update();
	}

	// Set current target angle.
	int x = 0;
	int y = 0;
	SDL_GetMouseState( &x, &y );
	int screenWidth = screen->getWidth();
	int screenHeight = screen->getHeight();
	x -= screenWidth / 2;
	y -= screenHeight / 2;
	float targetAngle = 0.0;
	if( y == 0 ) {
		if( x < 0 )
			targetAngle = 90;
		else
			targetAngle = 270;		
	}
	else {
		targetAngle = atanf((float) x/y) * (180/M_PI);

		if( y > 0 )
			targetAngle += 180;
	}
	//hero->getWeaponSystem()->SetTarget( targetAngle );

	// Read all events off the queue.
	while( !game->isFinished() && !isFinished() && !isAborted() && SDL_PollEvent(&event) ) {
		switch( event.type ) {
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ) {
					case SDLK_ESCAPE:
						AbortBattle();
						break;

					case SDLK_COMMA:
						keyUp = true;
						break;

					case SDLK_o:
						keyDown = true;
						break;

					case SDLK_a:
						keyLeft = true;
						break;

					case SDLK_u:
						keyRight = true;
						break;

					case SDLK_p:
						game->pause();
						break;

					default:
						break;
				}
				break;

			case SDL_KEYUP:
				switch( event.key.keysym.sym ) {
					case SDLK_COMMA:
						keyUp = false;
						break;

					case SDLK_o:
						keyDown = false;
						break;

					case SDLK_a:
						keyLeft = false;
						break;

					case SDLK_u:
						keyRight = false;
						break;

					default:
						break;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				if( event.button.button == SDL_BUTTON_LEFT )
					hero->startFiring();
				break;

			case SDL_MOUSEBUTTONUP:
				if( event.button.button == SDL_BUTTON_LEFT )
					hero->stopFiring();
				else if( event.button.button == SDL_BUTTON_WHEELUP )
					screen->setFOVY( screen->getFOVY() - 2 );
				else if( event.button.button == SDL_BUTTON_WHEELDOWN )
					screen->setFOVY( screen->getFOVY() + 2 );
				break;

			case SDL_QUIT:
				game->exitBT();
				break;

			default:
				break;
		}
	}
}


void Battle::Draw() {
	// Don't need to clear the screen, because the
	// entire area will be drawn again.  +20fps
	// If we're in wireframe mode, the clear will be necessary.
	if( game->getConfig()->getWireframe() )
		glClear( GL_COLOR_BUFFER_BIT );

	glClear( GL_DEPTH_BUFFER_BIT );

	// Set camera position and draw stuff...
	vec3 heroPos = hero->getPos();
	glLoadIdentity();
	glTranslatef( -heroPos[0], -heroPos[1], -15 );

	ground->Draw();
	//draw ground units?
	enemyAmmoList->DrawObjects();
	heroAmmoList->DrawObjects();
	enemies->DrawObjects();
	hero->Draw();
	explosionList->Draw();
	//draw power ups
	hud->Draw();
}


void Battle::FinishBattle() { finished = true; }
bool Battle::isFinished() { return finished; }

void Battle::AbortBattle() { aborted = true; }
bool Battle::isAborted() { return aborted; }
