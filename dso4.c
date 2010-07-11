/* ROBADV-- STEAL WINNER'S VALUABLES */

/*COPYRIGHT 1980, INFOCOM COMPUTERS AND COMMUNICATIONS, CAMBRIDGE MA. 02142*/
/* ALL RIGHTS RESERVED, COMMERCIAL USAGE STRICTLY PROHIBITED */
/* WRITTEN BY R. M. SUPNIK */

#include "funcs.h"
#include "vars.h"

integer robadv_(integer adv, integer nr, integer nc, integer na)
{
   /* System generated locals */
   integer ret_val;

   /* Local variables */
   integer i;

   ret_val = 0;
   /* 						!COUNT OBJECTS */
   for (i = 1; i <= objcts_1.olnt; ++i) {
      if (objcts_1.oadv[i - 1] != adv || objcts_1.otval[i - 1] <= 0 || (
               objcts_1.oflag2[i - 1] & SCRDBT) != 0) {
         continue;
      }
      newsta_(i, 0, nr, nc, na);
      /* 						!STEAL OBJECT */
      ++ret_val;
   }
   return ret_val;
} /* robadv_ */

/* ROBRM-- STEAL ROOM VALUABLES */

/* DECLARATIONS */

integer robrm_(integer rm, integer pr, integer nr, integer nc, integer na)
{
   /* System generated locals */
   integer ret_val;

   /* Local variables */
   integer i;

   ret_val = 0;
   /* 						!COUNT OBJECTS */
   for (i = 1; i <= objcts_1.olnt; ++i) {
      /* 						!LOOP ON OBJECTS. */
      if (! qhere_(i, rm)) {
         continue;
      }
      if (objcts_1.otval[i - 1] <= 0 || (objcts_1.oflag2[i - 1] & 
               SCRDBT) != 0 || (objcts_1.oflag1[i - 1] & 
                  VISIBT) == 0 || ! prob_(pr, pr)) {
         if ((objcts_1.oflag2[i - 1] & ACTRBT) != 0) {
            ret_val += robadv_(oactor_(i), nr, nc, na);
         }
      }
      else {
         newsta_(i, 0, nr, nc, na);
         ++ret_val;
         objcts_1.oflag2[i - 1] |= TCHBT;
         continue;
      }
   }
   return ret_val;
} /* robrm_ */

/* WINNIN-- SEE IF VILLAIN IS WINNING */

/* DECLARATIONS */

logical winnin_(integer vl, integer hr)
{
   /* System generated locals */
   logical ret_val;

   /* Local variables */
   integer ps, vs;

   vs = objcts_1.ocapac[vl - 1];
   /* 						!VILLAIN STRENGTH */
   ps = vs - fights_(hr, 1);
   /* 						!HIS MARGIN OVER HERO */
   if (ps > 3) {
      ret_val = prob_(90, 100);
   }
   /* 						!+3... 90% WINNING */
   else if (ps > 0) {
      ret_val = prob_(75, 85);
   }
   /* 						!>0... 75% WINNING */
   else if (ps == 0) {
      ret_val = prob_(50, 30);
   }
   /* 						!=0... 50% WINNING */
   else if (vs > 1) {
      ret_val = prob_(25, 25);
   }
   /* 						!ANY VILLAIN STRENGTH. */
   else {
      ret_val = prob_(10, 0);
   }
   return ret_val;
} /* winnin_ */

/* FIGHTS-- COMPUTE FIGHT STRENGTH */

/* DECLARATIONS */

integer fights_(integer h, logical flg)
{
   /* Initialized data */

   const integer smin = 2;
   const integer smax = 7;

   /* System generated locals */
   integer ret_val;

   ret_val = smin + ((smax - smin) * advs_1.ascore[h - 1] + state_1.mxscor /
         2) / state_1.mxscor;
   if (flg) {
      ret_val += advs_1.astren[h - 1];
   }
   return ret_val;
} /* fights_ */

/* VILSTR-	COMPUTE VILLAIN STRENGTH */

/* DECLARATIONS */

integer vilstr_(integer v)
{
   /* System generated locals */
   integer ret_val, i__1, i__2, i__3;

   /* Local variables */
   integer i;

   ret_val = objcts_1.ocapac[v - 1];
   if (ret_val <= 0) {
      return ret_val;
   }
   if (v == oindex_1.thief && findex_1.thfenf) {
      findex_1.thfenf = FALSE_;
      /* 						!THIEF UNENGROSSED. */
      ret_val = min(ret_val,2);
      /* 						!NO BETTER THAN 2. */
   }
   for (i = 1; i <= vill_1.vlnt; ++i) {
      /* 						!SEE IF  BEST WEAPON. */
      if (vill_1.villns[i - 1] == v && prsvec_1.prsi == vill_1.vbest[i - 1]
         ) {
         /* Computing MAX */
         i__2 = 1, i__3 = ret_val - 1;
         ret_val = max(i__2,i__3);
      }
   }
   return ret_val;
} /* vilstr_ */
