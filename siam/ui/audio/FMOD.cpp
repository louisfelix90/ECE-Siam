#include "../../inc/ui/audio/FMOD.hpp"

using namespace std;
using namespace Siam::UI::Audio;

FMOD::FMOD() {
	// Initialisation du système avec 10 cannaux
	FMOD_System_Create( &this->m_system );
	
	FMOD_System_Init( this->m_system, 10, FMOD_INIT_NORMAL, 0 );
	FMOD_System_GetMasterChannelGroup( this->m_system, &this->m_masterChannel );
	FMOD_System_GetChannel( this->m_system, 1, &this->m_musicChannel );
	FMOD_System_GetChannel( this->m_system, 2, &this->m_soundChannel );
}

FMOD::~FMOD() {
	// Libération des sons
	for( auto& sound : m_sounds ) {
		FMOD_Sound_Release( sound.second );
	}
	for( auto& music : m_musics ) {
		FMOD_Sound_Release( music.second );
	}

	// Libération du canal principal
	FMOD_ChannelGroup_Release( this->m_masterChannel );
	
	// Libération du système
	FMOD_System_Close( this->m_system );
	FMOD_System_Release( this->m_system );
}

bool FMOD::loadSound( string name, string filepath ) {
	// Chargement de tous les sons, si on n'y arrive pas on quitte le sous-programme.
	FMOD_RESULT ret;
	FMOD_SOUND* sound;

	ret = FMOD_System_CreateSound( this->m_system, filepath.c_str(), FMOD_CREATESAMPLE, 0, &sound );
	if( ret != FMOD_OK )
		return false;
	this->m_sounds[ name ] = sound;

	return true;
}

bool FMOD::loadMusic( string name, string filepath ) {
	// Chargement de tous les sons, si on n'y arrive pas on quitte le sous-programme.
	FMOD_RESULT ret;
	FMOD_SOUND* sound;

	ret = FMOD_System_CreateSound( this->m_system, filepath.c_str(), FMOD_2D | FMOD_CREATESTREAM, 0, &sound );
	if( ret != FMOD_OK )
		return false;
	this->m_musics[ name ] = sound;

	return true;
}

void FMOD::playSound( string name ) {
	FMOD_System_PlaySound( this->m_system, this->m_sounds[ name ], this->m_masterChannel, 0, &this->m_soundChannel );
}

void FMOD::playMusic( string name ) {
	FMOD_Sound_SetLoopCount( this->m_musics[ name ], -1 );
	FMOD_System_PlaySound( this->m_system, this->m_musics[ name ], this->m_masterChannel, 0, &this->m_soundChannel );
}

void FMOD::pauseMusic() {
	FMOD_BOOL state;
	FMOD_Channel_GetPaused( this->m_musicChannel, &state );
	FMOD_Channel_SetPaused( this->m_musicChannel, !state );
}

void FMOD::stopMusic() {
	FMOD_Channel_Stop( this->m_musicChannel );
}