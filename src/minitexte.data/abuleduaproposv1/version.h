#ifndef VERSION_H
#define VERSION_H

#define VER_FILEVERSION             1,0,0,0
#define VER_FILEVERSION_STR         "1.0.0\0"

#define VER_PRODUCTVERSION          1,0,0,0
#define VER_PRODUCTVERSION_STR      "1.0.0\0"

/* Attention fichier commun pour MiniTexte/Microtexte */
#ifdef __MICROTEXTE_MODE__
    #define VER_FILEDESCRIPTION_STR     "Version maternelle de Minitexte"
    #define VER_INTERNALNAME_STR        "abuledu-microtexte"
    #define VER_ORIGINALFILENAME_STR    "abuledu-microtexte.exe"
    #define VER_PRODUCTNAME_STR         "AbulEdu MicroTexte"
#else
    #define VER_FILEDESCRIPTION_STR     "Tout petit editeur de texte"
    #define VER_INTERNALNAME_STR        "abuledu-minitexte"
    #define VER_ORIGINALFILENAME_STR    "abuledu-minitexte.exe"
    #define VER_PRODUCTNAME_STR         "AbulEdu MiniTexte"
#endif


/* Parties communes entre les 2 projets */
#define VER_COMPANYNAME_STR         "RyXeo SARL"
#define VER_LEGALCOPYRIGHT_STR      "Copyright (c) 2012 RyXeo SARL"
#define VER_LEGALTRADEMARKS1_STR    "All Rights Reserved"
#define VER_LEGALTRADEMARKS2_STR    "License GNU/GPL v3"
#define VER_COMPANYDOMAIN_STR       "abuledu.org"

/** 20130624 : Ajout du numéro de version du module actuellement généré par l'application */
#define VER_UNITVERSION_STR         "1.0"

#endif // VERSION_H
