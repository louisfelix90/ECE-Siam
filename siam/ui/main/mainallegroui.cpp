#include "../../inc/ui/main/mainallegroui.hpp"

using namespace Siam;
using namespace Siam::UI::Main;
using namespace Siam::UI::Audio;

Allegro::Allegro( Siam::UI::Audio::FMOD& fmod ) : m_fmod( fmod ) {
	allegro_init();
	install_mouse();
	install_keyboard();

	jpgalleg_init();
	register_png_file_type();
	install_keyboard();
	install_mouse();

	set_color_depth( desktop_color_depth() );
	if( set_gfx_mode( GFX_AUTODETECT_WINDOWED, 1280, 720, 0, 0 ) != 0 ) {
		allegro_message( "probleme mode graphique" );
		allegro_exit();
		throw std::runtime_error( "Allegro initialization failed" );
	}

	set_window_title( "ECE-Siam" );

	splashscreen();

	enable_hardware_cursor();
	select_mouse_cursor( 2 );
	show_mouse( screen );

	loadSprites();
	loadBackgrounds();
	loadFonts();
	#ifdef USE_DUI
    loadPlayersDialog();
    #endif

    splashscreenEnd();

	m_page = create_bitmap( SCREEN_W, SCREEN_H );
	display();

	this->m_fmod.playMusic( "menu" );

	menu();

	freeBitmaps();
	allegro_exit();
	#ifdef USE_DUI
    exit( 0 );
    #endif
}

void Allegro::loadSprites() {
	std::string path;
	BITMAP* bmp;
	std::vector<std::string> bmpNames = {
		"logo",
	    "menu0",
		"menu1",
		"menu2",
		"menu3",
		"menu4",
	    "menuCol"
	};

	for( const auto& bmpName : bmpNames ) {
		path = "images/menu/";
		path += bmpName;
		path += ".png";

		bmp = load_bitmap( path.c_str(), NULL );
		if( bmp ) {
			this->m_bitmaps.insert( std::make_pair( bmpName, bmp ) );
		} else {
			path = "File not found: ";
			path = "images/menu/";
			path += bmpName;
			path += ".png";
			throw std::ios_base::failure( path );
		}
	}
}

void Allegro::loadBackgrounds() {
	std::string path;
	BITMAP* bmp;
	std::vector<std::string> bmpNames = {
		"mainBg"
	};

	for( const auto& bmpName : bmpNames ) {
		path = "images/menu/";
		path += bmpName;
		path += ".jpg";

		bmp = load_bitmap( path.c_str(), NULL );
		if( bmp ) {
			this->m_bitmaps.insert( std::make_pair( bmpName, bmp ) );
		} else {
			path = "File not found: ";
			path = "images/menu/";
			path += bmpName;
			path += ".png";
			throw std::ios_base::failure( path );
		}
	}
}

void Allegro::loadFonts() {
	this->m_textFont = load_font( "fonts/droidsans_14_mono.pcx", NULL, NULL );
	if( !this->m_textFont ) {
		throw std::ios_base::failure( "File not found: fonts/droidsans_14_mono.pcx" );
	}
	font = this->m_textFont;
}

void Allegro::freeBitmaps() {
	for( auto bmp = this->m_bitmaps.begin(); bmp != this->m_bitmaps.end(); bmp++ ) {
		if( bmp->second != nullptr ) {
			destroy_bitmap( bmp->second );
			bmp->second = nullptr;
		}
	}
}

void Allegro::splashscreen() {
	BITMAP* bg = load_jpg( "images/splashscreen.jpg", NULL );
	clear_bitmap( screen );
	blit( bg, screen, 0, 0, ( SCREEN_W - bg->w ) / 2, ( SCREEN_H - bg->h ) / 2, bg->w, bg->h );
	destroy_bitmap( bg );
}

void Allegro::splashscreenEnd() {
	std::this_thread::sleep_for( std::chrono::seconds( SPLASHCREEN_DURATION_AFTERLOADING ) );
}

void Allegro::display() {
	stretch_blit( this->m_bitmaps.find( "mainBg" )->second, this->m_page, 0, 0, this->m_bitmaps.find( "mainBg" )->second->w, this->m_bitmaps.find( "mainBg" )->second->h, 0, 0, SCREEN_W, SCREEN_H);

	set_alpha_blender();

	// Récupération de la couleur
	int item = getr( getpixel( this->m_bitmaps.find( "menuCol" )->second, mouse_x, mouse_y ) ) / 40;

	// Tests successifs des couleurs puis affichage à l'ecran d'un nouveau avant-plan et sélection du choix
	if( item == 1 ) {
		draw_trans_sprite( this->m_page, this->m_bitmaps.find( "menu1" )->second, 0, 0 );
		this->choice = item;
	} else if( item == 2 ) {
		/*draw_trans_sprite( this->m_page, this->m_bitmaps.find( "menu2" )->second, 0, 0 );
		this->choice = item;*/
		draw_trans_sprite( this->m_page, this->m_bitmaps.find( "menu0" )->second, 0, 0 );
		this->choice = 0;
	} else if( item == 3 ) {
		draw_trans_sprite( this->m_page, this->m_bitmaps.find( "menu3" )->second, 0, 0 );
		this->choice = item;
	} else if( item == 4 ) {
		draw_trans_sprite( this->m_page, this->m_bitmaps.find( "menu4" )->second, 0, 0 );
		this->choice = item;
	} else {
		draw_trans_sprite( this->m_page, this->m_bitmaps.find( "menu0" )->second, 0, 0 );
		this->choice = 0;
	}

	draw_trans_sprite( this->m_page, this->m_bitmaps.find( "logo" )->second, SCREEN_W - 50 - this->m_bitmaps.find( "logo" )->second->w, 100 );

	blit( this->m_page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
}

void Allegro::menu() {
	for( bool loop = true; loop; ) {
		this->prevMouseL = this->mouseL;
		this->mouseL = (bool) ( mouse_b & 1 );

		if( !this->prevMouseL && this->mouseL ) {
			switch( this->choice ) {
				case 1:
					// Nouvelle partie
					newGame();
					break;
				/*case 2:
					joinGame();
					break;*/
				case 3:
					// Options
					options();
					break;
				case 4:
					//Quitter
					loop = false;
					break;
				default:
					break;
			}
		}

		display();
	}
}

void Allegro::joinGame() {
	Functions::Allegro::Popup popup( "Non-implémenté" );
}

void Allegro::options() {
	std::vector<std::string> choices = { "Couper/Remettre les sons", "Retour" };
	Functions::Allegro::Popup pause = Functions::Allegro::Popup( "Options", choices, this->m_textFont );
	switch( pause.getChoice() ) {
		case 1:
			if( this->m_fmod.getMasterVolume() > 0 ) {
				this->m_fmod.setMasterVolume( 0 );
			} else {
				this->m_fmod.setMasterVolume( 1.0f );
			}
		default:
			break;
	}
}

#ifdef USE_DUI
void Allegro::loadPlayersDialog() {
	std::ostringstream thisptr;
	thisptr << (size_t) this;
	std::ifstream playersDialogFile( "ui/selectPlayers.dui" );
	if( !playersDialogFile.is_open() ) {
		throw std::ios_base::failure( "File not found: ui/selectPlayers.dui" );
	}
	std::stringstream playerDialogStream;
	playerDialogStream << playersDialogFile.rdbuf();
	std::string playersDialogString = playerDialogStream.str();
	size_t start_pos = playersDialogString.find( "allegptr" );
    if( start_pos != std::string::npos )
		playersDialogString.replace( start_pos, 8, thisptr.str() );
	std::stringstream playerDialogStreamModified;
	playerDialogStreamModified << playersDialogString;

	this->m_playersDialog = Dialog( playerDialogStreamModified );
	this->m_playersDialog.setCallback( "getPlayers", allegroGetPlayers );
}
#else
std::string Allegro::askPlayerName() {
    std::string name;
    int key;

    for( ; ; ) {
        stretch_blit( this->m_bitmaps.find( "mainBg" )->second, this->m_page, 0, 0, this->m_bitmaps.find( "mainBg" )->second->w, this->m_bitmaps.find( "mainBg" )->second->h, 0, 0, SCREEN_W, SCREEN_H );
        rectfill( this->m_page, ( SCREEN_W - DIALOG_PLAYERS_WIDTH ) / 2, ( SCREEN_H - DIALOG_PLAYERS_HEIGHT ) / 2 - DIALOG_PLAYERS_PADDING_H, ( SCREEN_W + DIALOG_PLAYERS_WIDTH ) / 2, ( SCREEN_H + DIALOG_PLAYERS_HEIGHT ) / 2 + DIALOG_PLAYERS_PADDING_H, makecol( 200, 200, 200 ) );

        textprintf_centre_ex( this->m_page, this->m_textFont, SCREEN_W / 2, SCREEN_H / 2 - this->m_textFont->height, makecol( 0, 0, 0 ), -1, "Nom du joueur:" );
        textprintf_centre_ex( this->m_page, this->m_textFont, SCREEN_W / 2, SCREEN_H / 2 + this->m_textFont->height, makecol( 0, 0, 0 ), -1, "%s", name.c_str() );

        blit( this->m_page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );

        key = readkey();
        if( ( ( key & 0xff ) >= 'a' && ( key & 0xff ) <= 'z') || ( ( key & 0xff ) >= 'A' && ( key & 0xff ) <= 'Z' ) ) {
            if( name.size() < 16 )
                name += (char) ( key & 0xff );
        } else if( ( key >> 8 ) == KEY_BACKSPACE ) {
            name.pop_back();
        } else if( ( key >> 8 ) == KEY_ENTER ) {
            return name;
        }  else if( ( key >> 8 ) == KEY_ESC ) {
            return "";
        }
    };
}
#endif

void Allegro::newGame() {
	this->m_players.clear();

	#ifdef USE_DUI
	stretch_blit( this->m_bitmaps.find( "mainBg" )->second, screen, 0, 0, this->m_bitmaps.find( "mainBg" )->second->w, this->m_bitmaps.find( "mainBg" )->second->h, 0, 0, SCREEN_W, SCREEN_H );
	rectfill( screen, ( SCREEN_W - DIALOG_PLAYERS_WIDTH ) / 2, ( SCREEN_H - DIALOG_PLAYERS_HEIGHT ) / 2 - DIALOG_PLAYERS_PADDING_H, ( SCREEN_W + DIALOG_PLAYERS_WIDTH ) / 2, ( SCREEN_H + DIALOG_PLAYERS_HEIGHT ) / 2 + DIALOG_PLAYERS_PADDING_H, makecol( 200, 200, 200 ) );

	centre_dialog( &this->m_playersDialog.dialog[ 0 ] );

	this->m_playersDialog.doDialog();
	#else
    std::string player1name = askPlayerName();
    if( player1name == "" )
        return;
    std::string player2name = askPlayerName();
    if( player2name == "" )
        return;
    this->m_players.push_back( Player( player1name, Objects::Types::Type::Rhinoceros ) );
	this->m_players.push_back( Player( player2name, Objects::Types::Type::Elephant ) );
	#endif

	if( this->m_players.size() > 0 ) {
		this->m_fmod.stopMusic();

		Siam::Game game( this->m_players, this->m_fmod, true );

		this->m_fmod.playMusic( "menu" );
	}
}

#ifdef USE_DUI
int Siam::UI::Main::allegroGetPlayers( Dialog::CallbackData cd ) {
	std::istringstream allegptr;
	size_t allegint;
	Allegro* alleg = nullptr;
	std::string player1name, player2name;

	player1name = Dialog::registry( 0 );
	player2name = Dialog::registry( 1 );

	/*Dialog *d = Dialog::getActiveDialog();
	bool player1local = Xml::find( d->xmlCache, "id", "player1local" )[ 0 ]->attr[ "flags" ].str != "0";
	bool player1ai = Xml::find( d->xmlCache, "id", "player1ai" )[ 0 ]->attr[ "flags" ].str != "0";
	bool player2local = Xml::find( d->xmlCache, "id", "player2local" )[ 0 ]->attr[ "flags" ].str != "0";
	bool player2ai = Xml::find( d->xmlCache, "id", "player2ai" )[ 0 ]->attr[ "flags" ].str != "0";*/

	allegptr.str( cd.parameters[ 0 ] );
	allegptr >> allegint;
	alleg = (Allegro*) allegint;

	if( player1name != "" && player2name != "" ) {
		alleg->m_players.push_back( Player( player1name, Objects::Types::Type::Rhinoceros ) );
		alleg->m_players.push_back( Player( player2name, Objects::Types::Type::Elephant ) );
	}

	return D_EXIT;
}
#endif
