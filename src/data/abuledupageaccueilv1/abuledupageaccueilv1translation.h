#include <QObject>
#include "abuledupageaccueilv1.h"

class AbulEduPageAccueilV1Translation : public QObject
{
   /** Special pour traductions : qt translator pourra extraire ces chaines a traduire
    * attention, le contexte AbulEduPageAccueilV1 est important pour que la localisation
    * s'applique bien Ã  l'objet AbulEduPageAccueilV1
    */
   QString a0000 = AbulEduPageAccueilV1::trUtf8("Survol");
   QString a0001 = AbulEduPageAccueilV1::trUtf8("Clic");
   QString a0002 = AbulEduPageAccueilV1::trUtf8("Double-clic");
   QString a0003 = AbulEduPageAccueilV1::trUtf8("Parcours");
}
