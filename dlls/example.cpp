/*
EXAMPLE GUN CODE
*/
#include "extdll.h" 
#include "util.h" 
#include "cbase.h" 
#include "monsters.h" 
#include "weapons.h" 
#include "nodes.h" 
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
enum mp5_e {
        MP5_LONGIDLE = 0,
        MP5_IDLE1,
        MP5_LAUNCH,
        MP5_RELOAD,
        MP5_DEPLOY,
        MP5_FIRE1,
        MP5_FIRE2,
        MP5_FIRE3
};

LINK_ENTITY_TO_CLASS( weapon_example, CExampleGun );

void CExampleGun::Spawn( ) {
   pev->classname = MAKE_STRING("weapon_example");
   Precache( );
   SET_MODEL(ENT(pev), "models/w_9mmAR.mdl");
   m_iId = WEAPON_EXAMPLE;
   m_iDefaultAmmo = 75;
   FallInit();
}
void CExampleGun::Precache( void ) {
   PRECACHE_MODEL("models/v_9mmAR.mdl");
   PRECACHE_MODEL("models/w_9mmAR.mdl");
   PRECACHE_MODEL("models/p_9mmAR.mdl");
   PRECACHE_SOUND("items/9mmclip1.wav");
   PRECACHE_SOUND("items/clipinsert1.wav");
   PRECACHE_SOUND("items/cliprelease1.wav");
   PRECACHE_SOUND ("weapons/hks1.wav");
   PRECACHE_SOUND ("weapons/hks2.wav");
   PRECACHE_SOUND ("weapons/hks3.wav");
   m_usMP5 = PRECACHE_EVENT( 1, "events/mp5.sc" );
}
void CExampleGun::PrimaryAttack()
{
	if( m_iClip <= 0 )
        {
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.05;
		return;
        }

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	m_pPlayer->pev->effects = (int)( m_pPlayer->pev->effects ) | EF_MUZZLEFLASH;

       	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_pPlayer->GetGunPosition();
        Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
        Vector vecDir;
#ifdef CLIENT_DLL
	if( !bIsMultiplayer() )
#else
	if( !g_pGameRules->IsMultiplayer() )
#endif
	{
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_MP5, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed );
        }

        int flags;
#if defined( CLIENT_WEAPONS )
        flags = FEV_NOTHOST;
#else
        flags = 0;
#endif
	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usMP5, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if( !m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		m_pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );

	m_flNextPrimaryAttack = GetNextAttackDelay( 0.1 );

	if( m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}
void CExampleGun::Reload( void )
{
if( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == CExampleGun_MAX_CLIP )
		return;
	DefaultReload( CExampleGun_MAX_CLIP, MP5_RELOAD, 1.5 );
}
void CExampleGun::WeaponIdle( void )
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	switch( RANDOM_LONG( 0, 1 ) )
        {
        case 0:
            iAnim = MP5_LONGIDLE;
            break;
        default:
        case 1:
            iAnim = MP5_IDLE1;
            break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}

BOOL CExampleGun::Deploy()
{
        return DefaultDeploy( "models/v_9mmAR.mdl", "models/p_9mmAR.mdl", MP5_DEPLOY, "mp5" );
}	

int CExampleGun::GetItemInfo(ItemInfo *p) {
   p->pszName = STRING(pev->classname);
   p->pszAmmo1 = "9mm";
   p->iMaxAmmo1 = CExampleGun_MAX_CLIP;
   p->iMaxClip = CExampleGun_MAX_CLIP;
   p->iSlot = 0;
   p->iPosition = 1;
   p->iFlags = 0;
   p->iId = m_iId = WEAPON_EXAMPLE;
   p->iWeight = MP5_WEIGHT; 
   p->pszAmmo2 = "NULL";
   p->iMaxAmmo2 = -1;
   return 1;
}
int CExampleGun::AddToPlayer( CBasePlayer *pPlayer ) {
   if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
   {
     MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
     WRITE_BYTE( m_iId );
     MESSAGE_END();
     return TRUE;
   }
   return FALSE;
}
