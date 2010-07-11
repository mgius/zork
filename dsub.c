/* RESIDENT SUBROUTINES FOR DUNGEON */

/*COPYRIGHT 1980, INFOCOM COMPUTERS AND COMMUNICATIONS, CAMBRIDGE MA. 02142*/
/* ALL RIGHTS RESERVED, COMMERCIAL USAGE STRICTLY PROHIBITED */
/* WRITTEN BY R. M. SUPNIK */

#include <stdio.h>
#include "funcs.h"
#include "vars.h"

#ifndef SEEK_SET
#define SEEK_SET (0)
#endif

extern FILE *dbfile;

static void rspsb2nl_ P((integer, integer, integer, logical));

/* RSPEAK-- OUTPUT RANDOM MESSAGE ROUTINE */

/* CALLED BY-- */

/* 	CALL RSPEAK(MSGNUM) */

void rspeak_(n)
integer n;
{
   rspsb2nl_(n, 0, 0, 1);
} /* rspeak_ */

/* RSPSUB-- OUTPUT RANDOM MESSAGE WITH SUBSTITUTABLE ARGUMENT */

/* CALLED BY-- */

/* 	CALL RSPSUB(MSGNUM,SUBNUM) */

void rspsub_(n, s1)
integer n;
integer s1;
{
   rspsb2nl_(n, s1, 0, 1);
} /* rspsub_ */

/* RSPSB2-- OUTPUT RANDOM MESSAGE WITH UP TO TWO SUBSTITUTABLE ARGUMENTS */

/* CALLED BY-- */

/* 	CALL RSPSB2(MSGNUM,SUBNUM1,SUBNUM2) */

void rspsb2_(n, s1, s2)
integer n;
integer s1;
integer s2;
{
   rspsb2nl_(n, s1, s2, 1);
} /* rspsb2_ */

/* rspsb2nl_ Display a substitutable message with an optional newline */

static void rspsb2nl_(integer n, integer y, integer z, integer nl)
{
   const char *zkey = "IanLanceTaylorJr";
   long x = (long)n;

   if (x > 0) {
      x = rmsg_1.rtext[x - 1];
   }
   /* 						!IF >0, LOOK UP IN RTEXT. */
   else if (x == 0) {
      return;
   }
   /* 						!ANYTHING TO DO? */
   play_1.telflg = TRUE_;
   /* 						!SAID SOMETHING. */

   x = ((- x) - 1) * 8;
#ifdef DEBUG
   if (fseek(dbfile, x + (long)rmsg_1.mrloc, SEEK_SET) == EOF) {
      fprintf(stderr, "Error seeking database loc %d\n", x);
      exit_();
   }
#endif

   if (nl)
      more_output(NULL);

   while (1) {
      integer i;

      i = getc(dbfile);
#ifdef DEBUG
      if (i == EOF) {
         fprintf(stderr, "Error reading database loc %d\n", x);
         exit_();
      }
#endif
      i ^= zkey[x & 0xf] ^ (x & 0xff);
      x = x + 1;
      if (i == '\0')
         break;
      else if (i == '\n') {
         putchar('\n');
         if (nl)
            more_output(NULL);
      }
      else if (i == '#' && y != 0) {
         long iloc;

         iloc = ftell(dbfile);
         rspsb2nl_(y, 0, 0, 0);
#ifdef DEBUG
         if (fseek(dbfile, iloc, SEEK_SET) == EOF) {
            fprintf(stderr, "Error seeking database loc %d\n", iloc);
            exit_();
         }
#endif
         y = z;
         z = 0;
      }
      else
         putchar(i);
   }

   if (nl)
      putchar('\n');
}

/* OBJACT-- APPLY OBJECTS FROM PARSE VECTOR */

/* DECLARATIONS */

logical objact_()
{
   /* 						!ASSUME WINS. */
   if (prsvec_1.prsi != 0 && 
         oappli_(objcts_1.oactio[prsvec_1.prsi - 1], 0)) {
      return TRUE_;
   }
   /* 						!YES, LET IT HANDLE. */

   /* 						!DIR OBJECT? */
   if (prsvec_1.prso != 0 && 
         oappli_(objcts_1.oactio[prsvec_1.prso - 1], 0)) {
      return TRUE_;
   }
   /* 						!YES, LET IT HANDLE. */

   return FALSE_;
} /* objact_ */

/* BUG-- REPORT FATAL SYSTEM ERROR */

/* CALLED BY-- */

/* 	CALL BUG(NO,PAR) */

void bug_(integer a, integer b)
{
#ifdef DEBUG

   /* Local variables */

   more_output(NULL);
   printf("PROGRAM ERROR %d, PARAMETER=%d\n", a, b);

   if (debug_1.dbgflg != 0) {
      return;
   }
   exit_();
#endif

} /* bug_ */

/* NEWSTA-- SET NEW STATUS FOR OBJECT */

/* CALLED BY-- */

/* 	CALL NEWSTA(OBJECT,STRING,NEWROOM,NEWCON,NEWADV) */

void newsta_(integer o, integer r, integer rm, integer cn, integer ad)
{
   rspeak_(r);
   objcts_1.oroom[o - 1] = rm;
   objcts_1.ocan[o - 1] = cn;
   objcts_1.oadv[o - 1] = ad;
} /* newsta_ */

/* QHERE-- TEST FOR OBJECT IN ROOM */

/* DECLARATIONS */

logical qhere_(integer obj, integer rm)
{
   /* Local variables */
   integer i;

   if (objcts_1.oroom[obj - 1] == rm) {
      return TRUE_;
   }
   /* 						!IN ROOM? */
   for (i = 1; i <= oroom2_1.r2lnt; ++i) {
      /* 						!NO, SCH ROOM2. */
      if (oroom2_1.oroom2[i - 1] == obj && oroom2_1.rroom2[i - 1] == rm) {
         return TRUE_;
      }
   }
   /* 						!NOT PRESENT. */
   return FALSE_;
} /* qhere_ */

/* QEMPTY-- TEST FOR OBJECT EMPTY */

/* DECLARATIONS */

logical qempty_(integer obj)
{
   /* Local variables */
   integer i;

   /* 						!ASSUME LOSE. */
   for (i = 1; i <= objcts_1.olnt; ++i) {
      if (objcts_1.ocan[i - 1] == obj) {
         return FALSE_;
      }
      /* 						!INSIDE TARGET? */
   }
   return TRUE_;
} /* qempty_ */

/* JIGSUP- YOU ARE DEAD */

/* DECLARATIONS */

void jigsup_(integer desc)
{
   /* Initialized data */

   static const integer rlist[9] = { 8,6,36,35,34,4,34,6,5 };

   /* Local variables */
   integer nonofl;
   logical f;
   integer i, j;

   rspeak_(desc);
   /* 						!DESCRIBE SAD STATE. */
   prsvec_1.prscon = 1;
   /* 						!STOP PARSER. */
#ifdef DEBUG
   if (debug_1.dbgflg != 0) {
      return;
   }
#endif
   /* 						!IF DBG, EXIT. */
   advs_1.avehic[play_1.winner - 1] = 0;
   /* 						!GET RID OF VEHICLE. */
   if (!(play_1.winner == aindex_1.player)) {
      /* 						!HIMSELF? */
      rspsub_(432, objcts_1.odesc2[advs_1.aobj[play_1.winner - 1] - 1]);
      /* 						!NO, SAY WHO DIED. */
      newsta_(advs_1.aobj[play_1.winner - 1], 0, 0, 0, 0);
      /* 						!SEND TO HYPER SPACE. */
      return;
   }

   if (findex_1.endgmf) {
      goto L900;
   }
   /* 						!NO RECOVERY IN END GAME. */
   if (state_1.deaths >= 2) {
      goto L1000;
   }
   /* 						!DEAD TWICE? KICK HIM OFF. */
   if (! yesno_(10, 9, 8)) {
      goto L1100;
   }
   /* 						!CONTINUE? */

   for (j = 1; j <= objcts_1.olnt; ++j) {
      /* 						!TURN OFF FIGHTING. */
      if (qhere_(j, play_1.here)) {
         objcts_1.oflag2[j - 1] &= ~ FITEBT;
      }
      /* L50: */
   }

   ++state_1.deaths;
   scrupd_(- 10);
   /* 						!CHARGE TEN POINTS. */
   f = moveto_(rindex_1.fore1, play_1.winner);
   /* 						!REPOSITION HIM. */
   findex_1.egyptf = TRUE_;
   /* 						!RESTORE COFFIN. */
   if (objcts_1.oadv[oindex_1.coffi - 1] == play_1.winner) {
      newsta_(oindex_1.coffi, 0, rindex_1.egypt, 0, 0);
   }
   objcts_1.oflag2[oindex_1.door - 1] &= ~ TCHBT;
   objcts_1.oflag1[oindex_1.robot - 1] = (objcts_1.oflag1[oindex_1.robot - 1]
         | VISIBT) & ~ NDSCBT;
   if (objcts_1.oroom[oindex_1.lamp - 1] != 0 || objcts_1.oadv[oindex_1.lamp 
         - 1] == play_1.winner) {
      newsta_(oindex_1.lamp, 0, rindex_1.lroom, 0, 0);
   }

   /* NOW REDISTRIBUTE HIS VALUABLES AND OTHER BELONGINGS. */

   /* THE LAMP HAS BEEN PLACED IN THE LIVING ROOM. */
   /* THE FIRST 8 NON-VALUABLES ARE PLACED IN LOCATIONS AROUND THE HOUSE. */
   /* HIS VALUABLES ARE PLACED AT THE END OF THE MAZE. */
   /* REMAINING NON-VALUABLES ARE PLACED AT THE END OF THE MAZE. */

   i = 1;
   for (j = 1; j <= objcts_1.olnt; ++j) {
      /* 						!LOOP THRU OBJECTS. */
      if (objcts_1.oadv[j - 1] != play_1.winner || objcts_1.otval[j - 1] != 
            0) {
         goto L200;
      }
      ++i;
      if (i > 9) {
         goto L400;
      }
      /* 						!MOVE TO RANDOM LOCATIONS. */
      newsta_(j, 0, rlist[i - 1], 0, 0);
L200:
      ;
   }

L400:
   i = rooms_1.rlnt + 1;
   /* 						!NOW MOVE VALUABLES. */
   nonofl = RAIR + RWATER + RSACRD + REND;
   /* 						!DONT MOVE HERE. */
   for (j = 1; j <= objcts_1.olnt; ++j) {
      if (objcts_1.oadv[j - 1] != play_1.winner || objcts_1.otval[j - 1] == 
            0) {
         goto L300;
      }
L250:
      --i;
      /* 						!FIND NEXT ROOM. */
      if ((rooms_1.rflag[i - 1] & nonofl) != 0) {
         goto L250;
      }
      newsta_(j, 0, i, 0, 0);
      /* 						!YES, MOVE. */
L300:
      ;
   }

   for (j = 1; j <= objcts_1.olnt; ++j) {
      /* 						!NOW GET RID OF REMAINDER. */
      if (objcts_1.oadv[j - 1] != play_1.winner) {
         goto L500;
      }
L450:
      --i;
      /* 						!FIND NEXT ROOM. */
      if ((rooms_1.rflag[i - 1] & nonofl) != 0) {
         goto L450;
      }
      newsta_(j, 0, i, 0, 0);
L500:
      ;
   }
   return;

   /* CAN'T OR WON'T CONTINUE, CLEAN UP AND EXIT. */

L900:
   rspeak_(625);
   /* 						!IN ENDGAME, LOSE. */
   goto L1100;

L1000:
   rspeak_(7);
   /* 						!INVOLUNTARY EXIT. */
L1100:
   score_(0);
   /* 						!TELL SCORE. */
   (void) fclose(dbfile);
   exit_();

} /* jigsup_ */

/* OACTOR-	GET ACTOR ASSOCIATED WITH OBJECT */

/* DECLARATIONS */

integer oactor_(integer obj)
{
   /* System generated locals */

   /* Local variables */
   integer i;

   for (i = 1; i <= advs_1.alnt; ++i) {
      /* 						!LOOP THRU ACTORS. */
      /* 						!ASSUME FOUND. */
      if (advs_1.aobj[i - 1] == obj) {
         return i;
      }
      /* 						!FOUND IT? */
      /* L100: */
   }
#ifdef DEBUG
   bug_(40, obj);
#endif
   /* 						!NO, DIE. */
   return advs_1.alnt + 1;
} /* oactor_ */

/* PROB-		COMPUTE PROBABILITY */

/* DECLARATIONS */

logical prob_(integer g, integer b)
{
   /* System generated locals */

   /* Local variables */
   integer i;

   i = g;
   /* 						!ASSUME GOOD LUCK. */
   if (findex_1.badlkf) {
      i = b;
   }
   /* 						!IF BAD, TOO BAD. */
   /* 						!COMPUTE. */
   return (rnd_(100) < i);
} /* prob_ */

/* RMDESC-- PRINT ROOM DESCRIPTION */

/* RMDESC PRINTS A DESCRIPTION OF THE CURRENT ROOM. */
/* IT IS ALSO THE PROCESSOR FOR VERBS 'LOOK' AND 'EXAMINE'. */

logical rmdesc_(full)
integer full;
{
   /* System generated locals */
   logical ret_val, L__1;

   /* Local variables */
   integer i, ra;

   /* FULL=	0/1/2/3=	SHORT/OBJ/ROOM/FULL */

   ret_val = TRUE_;
   /* 						!ASSUME WINS. */
   if (prsvec_1.prso < xsrch_1.xmin) {
      goto L50;
   }
   /* 						!IF DIRECTION, */
   screen_1.fromdr = prsvec_1.prso;
   /* 						!SAVE AND */
   prsvec_1.prso = 0;
   /* 						!CLEAR. */
L50:
   if (play_1.here == advs_1.aroom[aindex_1.player - 1]) {
      goto L100;
   }
   /* 						!PLAYER JUST MOVE? */
   rspeak_(2);
   /* 						!NO, JUST SAY DONE. */
   prsvec_1.prsa = vindex_1.walkiw;
   /* 						!SET UP WALK IN ACTION. */
   return ret_val;

L100:
   if (lit_(play_1.here)) {
      goto L300;
   }
   /* 						!LIT? */
   rspeak_(430);
   /* 						!WARN OF GRUE. */
   ret_val = FALSE_;
   return ret_val;

L300:
   ra = rooms_1.ractio[play_1.here - 1];
   /* 						!GET ROOM ACTION. */
   if (full == 1) {
      goto L600;
   }
   /* 						!OBJ ONLY? */
   i = rooms_1.rdesc2[play_1.here - 1];
   /* 						!ASSUME SHORT DESC. */
   if (full == 0 && (findex_1.superf || (rooms_1.rflag[play_1.here - 1] & 
               RSEEN) != 0 && findex_1.brieff)) {
      goto L400;
   }

   /*  The next line means that when you request VERBOSE mode, you */
   /*  only get long room descriptions 20% of the time. I don't either */
   /*  like or understand this, so the mod. ensures VERBOSE works */
   /*  all the time.			jmh@ukc.ac.uk 22/10/87 */

   /* &		        .AND.(BRIEFF.OR.PROB(80,80)))))       GO TO 400 */
   i = rooms_1.rdesc1[play_1.here - 1];
   /* 						!USE LONG. */
   if (i != 0 || ra == 0) {
      goto L400;
   }
   /* 						!IF GOT DESC, SKIP. */
   prsvec_1.prsa = vindex_1.lookw;
   /* 						!PRETEND LOOK AROUND. */
   if (! rappli_(ra)) {
      goto L100;
   }
   /* 						!ROOM HANDLES, NEW DESC? */
   prsvec_1.prsa = vindex_1.foow;
   /* 						!NOP PARSER. */
   goto L500;

L400:
   rspeak_(i);
   /* 						!OUTPUT DESCRIPTION. */
L500:
   if (advs_1.avehic[play_1.winner - 1] != 0) {
      rspsub_(431, objcts_1.odesc2[advs_1.avehic[play_1.winner - 1] - 
            1]);
   }

L600:
   if (full != 2) {
      L__1 = full != 0;
      princr_(L__1, play_1.here);
   }
   rooms_1.rflag[play_1.here - 1] |= RSEEN;
   if (full != 0 || ra == 0) {
      return ret_val;
   }
   /* 						!ANYTHING MORE? */
   prsvec_1.prsa = vindex_1.walkiw;
   /* 						!GIVE HIM A SURPISE. */
   if (! rappli_(ra)) {
      goto L100;
   }
   /* 						!ROOM HANDLES, NEW DESC? */
   prsvec_1.prsa = vindex_1.foow;
   return ret_val;

} /* rmdesc_ */

/* RAPPLI-	ROUTING ROUTINE FOR ROOM APPLICABLES */

/* DECLARATIONS */

logical rappli_(integer ri)
{
   /* Initialized data */

   const integer newrms = 38;

   /* System generated locals */

   /* 						!ASSUME WINS. */
   if (ri == 0) {
      return TRUE_;
   }
   /* 						!IF ZERO, WIN. */
   if (ri >= newrms) {
      return rappl2_(ri);
   }
   /* 						!IF NEW, PROCESSOR 2. */
   else if (ri < newrms) {
      return rappl1_(ri);
   }
   /* 						!IF OLD, PROCESSOR 1. */
} /* rappli_ */
