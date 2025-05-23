/************************************************************************/
/*                                                                      */
/*       Beispielprogramm:                              EVNT_MESAG( )   */
/*                                                                      */
/*       Copyright (c) 1989 Borland International                       */
/*       All rights reserved.                                           */
/*                                                                      */
/************************************************************************/

/* -------------------------------------------------------------------- */
/*       Include-Files einbinden.                                       */
/* -------------------------------------------------------------------- */

#include <aes.h>

/* -------------------------------------------------------------------- */
/*       Konstantendefinitionen.                                        */
/* -------------------------------------------------------------------- */

#define ME_HIDE               0
#define ME_SHOW               1
#define ME_NORM               1

#define RSC_NAME              "TCTEST.RSC"

/* -------------------------------------------------------------------- */
/*       Typendefinition.                                               */
/* -------------------------------------------------------------------- */

typedef enum
{
    FALSE,
    TRUE
}
boolean;

/* -------------------------------------------------------------------- */
/*       Globale Variablen.                                             */
/* -------------------------------------------------------------------- */

OBJECT *M_tree;

/* -------------------------------------------------------------------- */
/*       Funktionsprototypen.                                           */
/* -------------------------------------------------------------------- */

void gem_prg( void );

/* -------------------------------------------------------------------- */
/*       void gem_prg( void );                                          */
/*                                                                      */
/*       Beispiel f�r die Verwendung der Funktion evnt_mesag( ) in      */
/*       einem GEM-Programm.                                            */
/*                                                                      */
/*       ->                      Nichts.                                */
/*                                                                      */
/*       <-                      Nichts.                                */
/* -------------------------------------------------------------------- */

void gem_prg( void )
{
   int     msgbuff[8];                    /* Mitteilungspuffer.         */
   boolean done = FALSE;                  /* 'Mach weiter'-Flag.        */

   if ( rsrc_load( RSC_NAME ) > 0 )       /* Es kann auch ein anderes   */
   {                                      /* Rsc-File geladen werden.   */
      rsrc_gaddr( 0, 0, &M_tree );        /* Startadresse des Baumes    */
                                          /* bestimmen.                 */
      graf_mouse( M_OFF, 0 );             /* Maus ausblenden.           */
      menu_bar( M_tree, ME_SHOW );        /* Men� anzeigen.             */
      graf_mouse( M_ON, 0 );
      graf_mouse( ARROW, 0 );             /* Mauszeiger ist der Pfeil.  */

      do
      {
         evnt_mesag( msgbuff );           /* Ereignis abwarten.         */

         if ( msgbuff[0] == MN_SELECTED ) /* Men�punkt wurde ausge�hlt. */
         {
            if ( msgbuff[4] < 16 )        /* Kleiner 16 ist immer About */
                                          /* bzw. Accessory-Eintrag.    */
               form_alert( 1, "[0][     Turbo C Demo| |(c) 1989 \
Borland Int.  |All rights reserved.][ Ok ]" );
            else                          /* Wenn er eine h�here Nummer */
               done = TRUE;               /* besitzt, dann ist es voll- */
                                          /* bracht.                    */

            menu_tnormal( M_tree, msgbuff[3], ME_NORM );
         }                                /* Men�titel normal zeichnen. */
      }
      while ( !done );                    /* Solange nichts getan ist!  */

      graf_mouse( M_OFF,0 );
      menu_bar( M_tree, ME_HIDE );        /* Men� verschwinden lassen.  */
      graf_mouse( M_ON, 0 );
      rsrc_free( );                       /* Durch Rsc-Datei belegten   */
   }                                      /* Speicher freigeben.        */
   else
      form_alert( 1, "[3][Rsc-Datei nicht gefunden.][Abbruch]" );
}

/* -------------------------------------------------------------------- */
/*       Ende der Beispielprogramms f�r EVNT_MESAG( ).                  */
/* -------------------------------------------------------------------- */
