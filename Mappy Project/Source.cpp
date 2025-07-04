#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "SpriteSheet.h"
#include "mappy_A5.h"
#include <iostream>
using namespace std;

int collided(int x, int y);  //Tile Collision
bool endValue( int x, int y ); //End Block with the User Value = 8
bool dieValue(int x, int y); //end game with user = 5
int main(void)
{
	const int WIDTH = 1400;
	const int HEIGHT = 1000;
	bool keys[] = {false, false, false, false, false};
	enum KEYS{UP, DOWN, LEFT, RIGHT, SPACE};
	
	//variables
	bool done = false; //exit gameloop
	bool render = false;
	bool gameOver = false; //for ending game
	bool levelOver = false; //for changing levels
	int countdownTime = 60; //timer seconds
	char name[80];
	int level = 1; //set to level 1

	//Player Variable
	Sprite player;

	//allegro variable
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer;
	ALLEGRO_TIMER *countdown;

	//program init
	if(!al_init())										//initialize Allegro
		return -1;

	display = al_create_display(WIDTH, HEIGHT);			//create our display object

	if(!display)										//test display object
		return -1;

	//addon init
	al_install_keyboard();
	al_init_image_addon();
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();

	player.InitSprites(WIDTH,HEIGHT);

	int xOff = 0;
	int yOff = 0;
	if(MapLoad("level1.fmp", 1))
		return -5;
	MapInitAnims();

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / 60);
	countdown = al_create_timer(1.0); //created countdown timer
	

	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(countdown));
	al_start_timer(countdown);
	al_start_timer(timer);

	ALLEGRO_FONT* font = al_load_font("DFPPOPCorn-W12.ttf", 64, 0);

	//draw the background tiles
	MapDrawBG(xOff,yOff, 0, 0, WIDTH-1, HEIGHT-1);

	//draw foreground tiles
	MapDrawFG(xOff,yOff, 0, 0, WIDTH-1, HEIGHT-1, 0);
	player.DrawSprites(0,0);
	al_flip_display();
	al_clear_to_color(al_map_rgb(0,0,0));

	while (!done)
	{
			ALLEGRO_EVENT ev;
			al_wait_for_event(event_queue, &ev);

			if (ev.type == ALLEGRO_EVENT_TIMER)
			{
				if (ev.timer.source == countdown) {
					if (countdownTime > 0) {
						countdownTime -= 1;
					}
					else {
						gameOver = true;
					}
				}
				else {
					if (levelOver) {
						countdownTime = 60;
						levelOver = false;
						level++;
						sprintf(name, "level%i.FMP", level);
						MapLoad(name, 1);
						MapInitAnims();
						if (level < 4)
							player.resetPlayer();
					}

					render = true;
					if (keys[UP])
						player.UpdateSprites(WIDTH, HEIGHT, 3);
					else if (keys[DOWN])
						player.UpdateSprites(WIDTH, HEIGHT, 4);
					else if (keys[LEFT])
						player.UpdateSprites(WIDTH, HEIGHT, 0);
					else if (keys[RIGHT])
						player.UpdateSprites(WIDTH, HEIGHT, 1);
					else
						player.UpdateSprites(WIDTH, HEIGHT, 2);
					if (player.CollisionEndBlock()) {
						levelOver = true;
					}
					if (player.CollisionDieBlock())
						gameOver = true;

					render = true;
				}
			}
			else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				done = true;
			}
			else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch (ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_ESCAPE:
					done = true;
					break;
				case ALLEGRO_KEY_UP:
					keys[UP] = true;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = true;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = true;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = true;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = true;
					break;
				}
			}
			else if (ev.type == ALLEGRO_EVENT_KEY_UP)
			{
				switch (ev.keyboard.keycode)
				{
				case ALLEGRO_KEY_ESCAPE:
					done = true;
					break;
				case ALLEGRO_KEY_UP:
					keys[UP] = false;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = false;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = false;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = false;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = false;
					break;
				}
			}
			if (render && al_is_event_queue_empty(event_queue))
			{
				render = false;

				//update the map scroll position
				xOff = player.getX() + player.getWidth() - WIDTH / 2;
				yOff = player.getY() + player.getHeight() - HEIGHT / 2;

				//avoid moving beyond the map edge
				if (xOff < 0) xOff = 0;

				if (xOff > (mapwidth * mapblockwidth - WIDTH))
					xOff = mapwidth * mapblockwidth - WIDTH;
				if (yOff < 0)
					yOff = 0;
				if (yOff > (mapheight * mapblockheight - HEIGHT))
					yOff = mapheight * mapblockheight - HEIGHT;

				MapUpdateAnims();

				//draw the background tiles
				MapDrawBG(xOff, yOff, 0, 0, WIDTH, HEIGHT);

				//draw foreground tiles
				MapDrawFG(xOff, yOff, 0, 0, WIDTH, HEIGHT, 0);

				//if gameover via time or caught, draw ded sprite & end game
				if (gameOver) {
					player.UpdateSprites(WIDTH, HEIGHT, 5);
					player.DrawSprites(xOff, yOff);
					al_draw_text(font, al_map_rgb(255, 0, 0), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "GAME OVER!");
					al_flip_display();
					al_rest(5);
					done = true;
				}

				//else contunue drawing
				player.DrawSprites(xOff, yOff);

				//if the level's over & 3 has been completed, win the game!
				if (levelOver && level == 4) {
					al_draw_text(font, al_map_rgb(255, 0, 0), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "MARISA STOLE THE PRECIOUS THING!");
					al_flip_display();
					al_rest(5);
					done = true;
				}

				al_draw_textf(font, al_map_rgb(255, 255, 255), WIDTH, 10, ALLEGRO_ALIGN_RIGHT, "TIME: %i", countdownTime);

				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
			}
	}
	MapFreeMem();
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);						//destroy our display object
	al_destroy_font(font);

	return 0;
}



int collided(int x, int y)
{
	BLKSTR *blockdata;
	blockdata = MapGetBlock(x/mapblockwidth, y/mapblockheight);
	return blockdata->tl;
}

bool endValue( int x, int y )
{

	BLKSTR* data;
	data = MapGetBlock( x/mapblockwidth, y/mapblockheight );

	if( data->user1 == 8 )
	{
		return true;
	}else
		return false;
}
bool dieValue(int x, int y)
{

	BLKSTR* data;
	data = MapGetBlock(x / mapblockwidth, y / mapblockheight);

	if (data->user1 == 5)
	{
		return true;
	}
	else
		return false;
}