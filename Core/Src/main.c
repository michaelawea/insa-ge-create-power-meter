/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Application principale - Wattmètre numérique STM32L052K6
  ******************************************************************************
  * @attention
  *
  * Système de mesure de puissance électrique
  * - Microcontrôleur: STM32L052K6T6 (32KB Flash, 8KB RAM)
  * - Mesure tension/courant via canaux ADC
  * - Affichage OLED SSD1306 128x32 pixels
  * - Interface utilisateur avec encodeur rotatif
  *
  * Configuration des broches:
  * PA3 (ADC_CH3) - Entrée tension
  * PA4 (ADC_CH4) - Entrée courant  
  * PB3 - Bouton poussoir
  * PB4/PB5 - Encodeur rotatif
  * PB6/PB7 - Bus I2C (écran OLED)
  *
  * Copyright (c) 2024 STMicroelectronics.
  * Tous droits réservés.
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "ssd1306/ssd1306.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Constantes de calibration du système */
#define FACTEUR_ECHELLE_TENSION 2.15f    /* Diviseur de tension (30V max → 3.3V ADC) */
#define FACTEUR_ECHELLE_COURANT 3.59f    /* Ratio capteur de courant (5A max → 3.3V ADC) */
#define TENSION_REF_ADC         3.3f     /* Tension de référence ADC */
#define RESOLUTION_ADC          4095.0f  /* Résolution ADC 12 bits */

/* Paramètres d'optimisation mémoire */
#define POINTS_GRAPHIQUE        32       /* Nombre de points pour les graphiques */
#define DELAI_RETOUR_MENU       30000    /* Délai de retour automatique (30 secondes) */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim6;

/* USER CODE BEGIN PV */
/* Variables de l'encodeur rotatif */
static uint8_t etat_encodeur;
static uint8_t compteur_rotation;
static uint8_t etat_bouton;

/* Variables de mesure électrique */
static float tension_mesuree = 0.0f;      /* Tension mesurée en volts */
static float courant_mesure = 0.0f;       /* Courant mesuré en ampères */
static float puissance_calculee = 0.0f;   /* Puissance en watts */
static float energie_cumulee = 0.0f;      /* Énergie accumulée en Wh */
static uint32_t dernier_temps = 0;        /* Horodatage pour intégration */

/* Suivi des valeurs maximales */
static float tension_max = 0.0f;
static float courant_max = 0.0f;
static float puissance_max = 0.0f;

/* Gestion du bouton poussoir */
static uint32_t temps_appui_bouton = 0;
static uint8_t appui_long_traite = 0;
static uint32_t derniere_interruption_bouton = 0;
static uint8_t etat_stable_bouton = 0;

/* Énumération des états du menu */
typedef enum {
    ECRAN_WATTMETRE = 0,     /* Écran principal du wattmètre */
    ECRAN_MENU_PRINCIPAL,    /* Menu principal */
    ECRAN_VALEURS_MAX,       /* Affichage des valeurs maximales */
    ECRAN_GRAPHIQUES,        /* Affichage graphique */
    ECRAN_CHOIX_GRAPHIQUE,   /* Sélection du paramètre graphique */
    ECRAN_PARAMETRES,        /* Menu des paramètres */
    ECRAN_REINITIALISATION,  /* Menu de réinitialisation */
    ECRAN_INFORMATIONS       /* Informations système */
} EtatMenu_t;

static EtatMenu_t menu_actuel = ECRAN_WATTMETRE;
static uint8_t selection_menu = 0;
static uint8_t menu_modifie = 1;
static uint32_t derniere_activite = 0;

/* Variables pour l'anti-rebond de l'encodeur */
static uint32_t derniere_interruption_encodeur = 0;

/* Historique pour les graphiques (optimisé mémoire) */
static float historique_tension[POINTS_GRAPHIQUE];
static float historique_courant[POINTS_GRAPHIQUE];
static float historique_puissance[POINTS_GRAPHIQUE];
static uint8_t index_donnees_graphe = 0;
static uint8_t parametre_graphique = 0;  /* 0=Tension, 1=Courant, 2=Puissance */
static uint32_t derniere_maj_graphe = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  Conversion valeur ADC vers tension réelle
  * @param  valeur_adc Valeur ADC brute (0-4095)
  * @retval Tension réelle en volts (plage 0-30V)
  */
float Convertir_ADC_Tension(uint32_t valeur_adc)
{
    /* Canal ADC 3 (PA3): Mesure de tension avec diviseur résistif */
    /* Calcul en deux étapes pour meilleure précision */
    float tension_adc = (valeur_adc * TENSION_REF_ADC) / RESOLUTION_ADC;
    return tension_adc * FACTEUR_ECHELLE_TENSION;
}

/**
  * @brief  Conversion valeur ADC vers courant réel
  * @param  valeur_adc Valeur ADC brute (0-4095)
  * @retval Courant réel en ampères (plage 0-5A)
  */
float Convertir_ADC_Courant(uint32_t valeur_adc)
{
    /* Canal ADC 4 (PA4): Mesure de courant via capteur à effet Hall */
    /* Conversion avec compensation offset si nécessaire */
    float tension_capteur = valeur_adc * (TENSION_REF_ADC / RESOLUTION_ADC);
    return tension_capteur * FACTEUR_ECHELLE_COURANT;
}

/**
  * @brief  Calcul de la puissance électrique
  * @param  tension Tension mesurée en volts
  * @param  courant Courant mesuré en ampères
  * @retval Puissance calculée en watts
  */
float Calculer_Puissance(float tension, float courant)
{
    /* P = U × I (loi d'Ohm) */
    float puissance = tension * courant;
    /* Limiter la puissance à une valeur raisonnable */
    if (puissance > 150.0f) puissance = 150.0f;
    return puissance;
}

/**
  * @brief  Mise à jour de l'énergie accumulée par intégration
  * @param  puissance Puissance actuelle en watts
  * @param  intervalle_temps Intervalle de temps en millisecondes
  */
void Actualiser_Energie(float puissance, uint32_t intervalle_temps)
{
    /* Conversion du temps de ms en heures pour calcul en Wh */
    float heures = intervalle_temps / 3600000.0f;
    
    /* Intégration numérique: E = E + P × Δt */
    energie_cumulee += puissance * heures;
}

/**
  * @brief  Actualisation des valeurs maximales
  * @param  tension Valeur actuelle de tension
  * @param  courant Valeur actuelle de courant
  * @param  puissance Valeur actuelle de puissance
  */
void Actualiser_Maximums(float tension, float courant, float puissance)
{
    /* Mise à jour conditionnelle des maximums */
    tension_max = (tension > tension_max) ? tension : tension_max;
    courant_max = (courant > courant_max) ? courant : courant_max;
    puissance_max = (puissance > puissance_max) ? puissance : puissance_max;
}

/**
  * @brief  Réinitialisation des valeurs maximales
  */
void Reinitialiser_Maximums(void)
{
    /* Remise à zéro de toutes les valeurs max */
    tension_max = 0.0f;
    courant_max = 0.0f;
    puissance_max = 0.0f;
}

/**
  * @brief  Réinitialisation de l'énergie accumulée
  */
void Reinitialiser_Energie(void)
{
    /* RAZ du compteur d'énergie */
    energie_cumulee = 0.0f;
}

/**
  * @brief  Gestion de la navigation dans les menus via encodeur
  * @param  sens: 1 pour horaire, -1 pour anti-horaire
  */
void Gerer_Navigation_Menu(int8_t sens)
{
    derniere_activite = HAL_GetTick();
    menu_modifie = 1;

    switch (menu_actuel) {
        case ECRAN_MENU_PRINCIPAL:
            if (sens > 0) {
                selection_menu = (selection_menu + 1) % 5;
            } else {
                selection_menu = (selection_menu == 0) ? 4 : selection_menu - 1;
            }
            break;

        case ECRAN_CHOIX_GRAPHIQUE:
            /* Navigation circulaire dans 4 options */
            selection_menu = (sens > 0) ? 
                ((selection_menu + 1) % 4) : 
                ((selection_menu == 0) ? 3 : selection_menu - 1);
            break;

        case ECRAN_PARAMETRES:
            /* Basculement entre 2 options */
            selection_menu = (selection_menu == 0) ? 1 : 0;
            break;

        case ECRAN_REINITIALISATION:
            /* Navigation dans 3 options */
            if (sens > 0) {
                selection_menu++;
                if (selection_menu > 2) selection_menu = 0;
            } else {
                if (selection_menu == 0) selection_menu = 2;
                else selection_menu--;
            }
            break;

        default:
            break;
    }
}

/**
  * @brief  Traitement des actions du bouton dans les menus
  * @param  type_appui: 0 = appui court, 1 = appui long
  */
void Executer_Action_Menu(uint8_t type_appui)
{
    derniere_activite = HAL_GetTick();
    menu_modifie = 1;

    if (type_appui == 1) { /* Appui long - retour arrière */
        if (menu_actuel == ECRAN_WATTMETRE) {
            menu_actuel = ECRAN_MENU_PRINCIPAL;
            selection_menu = 0;
        } else {
            /* Retour à l'écran principal depuis n'importe où */
            menu_actuel = ECRAN_WATTMETRE;
            selection_menu = 0;
        }
    } else { /* Appui court - validation/entrée */
        switch (menu_actuel) {
            case ECRAN_WATTMETRE:
                /* Pas d'action sur appui court en mode wattmètre */
                break;

            case ECRAN_MENU_PRINCIPAL:
                /* Navigation selon la sélection */
                if (selection_menu == 0) menu_actuel = ECRAN_WATTMETRE;
                else if (selection_menu == 1) menu_actuel = ECRAN_VALEURS_MAX;
                else if (selection_menu == 2) {
                    menu_actuel = ECRAN_CHOIX_GRAPHIQUE;
                    selection_menu = 0;
                }
                else if (selection_menu == 3) {
                    menu_actuel = ECRAN_PARAMETRES;
                    selection_menu = 0;
                }
                else if (selection_menu == 4) {
                    menu_actuel = ECRAN_REINITIALISATION;
                    selection_menu = 0;
                }
                break;

            case ECRAN_VALEURS_MAX:
                menu_actuel = ECRAN_MENU_PRINCIPAL;
                selection_menu = 1;
                break;

            case ECRAN_CHOIX_GRAPHIQUE:
                if (selection_menu < 3) {
                    /* Sélection du paramètre à afficher */
                    parametre_graphique = selection_menu;
                    menu_actuel = ECRAN_GRAPHIQUES;
                } else {
                    /* Retour au menu principal */
                    menu_actuel = ECRAN_MENU_PRINCIPAL;
                    selection_menu = 2;
                }
                break;

            case ECRAN_GRAPHIQUES:
                menu_actuel = ECRAN_CHOIX_GRAPHIQUE;
                selection_menu = parametre_graphique;
                break;

            case ECRAN_PARAMETRES:
                if (selection_menu == 0) {
                    menu_actuel = ECRAN_INFORMATIONS;
                } else {
                    menu_actuel = ECRAN_MENU_PRINCIPAL;
                    selection_menu = 3;
                }
                break;

            case ECRAN_REINITIALISATION:
                switch (selection_menu) {
                    case 0: 
                        Reinitialiser_Maximums();
                        menu_actuel = ECRAN_MENU_PRINCIPAL;
                        selection_menu = 4;
                        break;
                    case 1: 
                        Reinitialiser_Energie();
                        menu_actuel = ECRAN_MENU_PRINCIPAL;
                        selection_menu = 4;
                        break;
                    case 2: 
                        menu_actuel = ECRAN_MENU_PRINCIPAL;
                        selection_menu = 4;
                        break;
                }
                break;

            case ECRAN_INFORMATIONS:
                menu_actuel = ECRAN_PARAMETRES;
                selection_menu = 0;
                break;
        }
    }
}

/**
  * @brief  Affichage du menu actuel sur écran OLED
  */
void Afficher_Menu_Actuel(void)
{
    char ligne1[21] = {0};
    char ligne2[21] = {0};
    char ligne3[21] = {0};

    ssd1306_Fill(Black);

    switch (menu_actuel) {
        case ECRAN_WATTMETRE:
            Afficher_Wattmetre();
            return;

        case ECRAN_MENU_PRINCIPAL:
            {
                const char* elements_menu[5] = {
                    " Wattmètre",
                    " Valeurs Max",
                    " Graphiques",
                    " Paramètres",
                    " Réinitialiser"
                };

                ssd1306_SetCursor(0, 0);
                ssd1306_WriteString("=== MENU PRINCIPAL ===", Font_6x8, White);

                uint8_t debut_affichage = 0;
                /* Ajustement de la fenêtre d'affichage */
                if (selection_menu >= 2) {
                    debut_affichage = selection_menu - 1;
                    if (debut_affichage > 2) debut_affichage = 2;
                }

                /* Affichage de 3 éléments maximum */
                for (uint8_t i = 0; i < 3 && (debut_affichage + i) < 5; i++) {
                    uint8_t index_element = debut_affichage + i;
                    char ligne_affichage[21];

                    sprintf(ligne_affichage, "%s%s",
                           (index_element == selection_menu) ? ">" : " ",
                           elements_menu[index_element]);

                    ssd1306_SetCursor(0, 8 + (i * 8));
                    ssd1306_WriteString(ligne_affichage, Font_6x8, White);
                }

                /* Indicateurs de défilement */
                if (debut_affichage > 0) {
                    ssd1306_SetCursor(120, 8);
                    ssd1306_WriteString("^", Font_6x8, White);
                }
                if (debut_affichage + 3 < 5) {
                    ssd1306_SetCursor(120, 24);
                    ssd1306_WriteString("v", Font_6x8, White);
                }
            }
            break;

        case ECRAN_VALEURS_MAX:
            ssd1306_SetCursor(0, 0);
            ssd1306_WriteString("=== VALEURS MAX ===", Font_6x8, White);

            /* Extraction des parties entières et décimales */
            int v_max_ent = (int)tension_max;
            int v_max_dec = (int)((tension_max - v_max_ent) * 10.0f);
            int i_max_ent = (int)courant_max;
            int i_max_dec = (int)((courant_max - i_max_ent) * 100.0f);
            int p_max_ent = (int)puissance_max;
            int p_max_dec = (int)((puissance_max - p_max_ent) * 10.0f);

            sprintf(ligne1, "U: %d,%dV", v_max_ent, v_max_dec);
            sprintf(ligne2, "I: %d,%02dA", i_max_ent, i_max_dec);
            sprintf(ligne3, "P: %d,%dW", p_max_ent, p_max_dec);

            ssd1306_SetCursor(0, 10);
            ssd1306_WriteString(ligne1, Font_7x10, White);
            ssd1306_SetCursor(0, 20);
            ssd1306_WriteString(ligne2, Font_7x10, White);
            ssd1306_SetCursor(70, 20);
            ssd1306_WriteString(ligne3, Font_7x10, White);
            break;

        case ECRAN_PARAMETRES:
            ssd1306_SetCursor(0, 0);
            ssd1306_WriteString("=== PARAMETRES ===", Font_6x8, White);

            sprintf(ligne1, "%s À propos", (selection_menu == 0) ? ">" : " ");
            sprintf(ligne2, "%s Retour", (selection_menu == 1) ? ">" : " ");

            ssd1306_SetCursor(0, 12);
            ssd1306_WriteString(ligne1, Font_7x10, White);
            ssd1306_SetCursor(0, 22);
            ssd1306_WriteString(ligne2, Font_7x10, White);
            break;

        case ECRAN_REINITIALISATION:
            ssd1306_SetCursor(0, 0);
            ssd1306_WriteString("=== REINITIALISER ===", Font_6x8, White);

            sprintf(ligne1, "%s RAZ Max", (selection_menu == 0) ? ">" : " ");
            sprintf(ligne2, "%s RAZ Énergie", (selection_menu == 1) ? ">" : " ");
            sprintf(ligne3, "%s Annuler", (selection_menu == 2) ? ">" : " ");

            ssd1306_SetCursor(0, 8);
            ssd1306_WriteString(ligne1, Font_6x8, White);
            ssd1306_SetCursor(0, 16);
            ssd1306_WriteString(ligne2, Font_6x8, White);
            ssd1306_SetCursor(0, 24);
            ssd1306_WriteString(ligne3, Font_6x8, White);
            break;

        case ECRAN_CHOIX_GRAPHIQUE:
            {
                const char* options_graphiques[4] = {
                    " Tension (V)",
                    " Courant (A)",
                    " Puissance (W)",
                    " Retour"
                };

                ssd1306_SetCursor(0, 0);
                ssd1306_WriteString("=== GRAPHIQUES ===", Font_6x8, White);

                uint8_t debut = 0;
                if (selection_menu >= 2) {
                    debut = selection_menu - 1;
                    if (debut > 1) debut = 1;
                }

                for (uint8_t i = 0; i < 3 && (debut + i) < 4; i++) {
                    uint8_t idx = debut + i;
                    char ligne[21];

                    sprintf(ligne, "%s%s",
                           (idx == selection_menu) ? ">" : " ",
                           options_graphiques[idx]);

                    ssd1306_SetCursor(0, 8 + (i * 8));
                    ssd1306_WriteString(ligne, Font_6x8, White);
                }

                if (debut > 0) {
                    ssd1306_SetCursor(120, 8);
                    ssd1306_WriteString("^", Font_6x8, White);
                }
                if (debut + 3 < 4) {
                    ssd1306_SetCursor(120, 24);
                    ssd1306_WriteString("v", Font_6x8, White);
                }
            }
            break;

        case ECRAN_GRAPHIQUES:
            Afficher_Graphique();
            return;

        case ECRAN_INFORMATIONS:
            ssd1306_SetCursor(0, 0);
            ssd1306_WriteString("Wattmètre v1.0", Font_7x10, White);
            ssd1306_SetCursor(0, 12);
            ssd1306_WriteString("STM32L052K6", Font_6x8, White);
            ssd1306_SetCursor(0, 20);
            ssd1306_WriteString("Carte Production", Font_6x8, White);
            ssd1306_SetCursor(0, 28);
            ssd1306_WriteString("Mesure Puissance", Font_6x8, White);
            break;
    }

    ssd1306_UpdateScreen();
}

/**
  * @brief  Mise à jour du tampon de données graphiques
  */
void Actualiser_Donnees_Graphe(void)
{
    uint32_t temps_actuel = HAL_GetTick();

    /* Mise à jour toutes les 200ms */
    if (temps_actuel - derniere_maj_graphe > 200) {
        historique_tension[index_donnees_graphe] = tension_mesuree;
        historique_courant[index_donnees_graphe] = courant_mesure;
        historique_puissance[index_donnees_graphe] = puissance_calculee;

        /* Incrémentation circulaire de l'index */
        index_donnees_graphe = (index_donnees_graphe + 1) % POINTS_GRAPHIQUE;
        derniere_maj_graphe = temps_actuel;
    }
}

/**
  * @brief  Affichage des courbes graphiques
  */
void Afficher_Graphique(void)
{
    char titre[21] = {0};
    float valeur_max = 0.0f;
    float* donnees;

    ssd1306_Fill(Black);

    /* Sélection des données selon le paramètre */
    switch (parametre_graphique) {
        case 0: { /* Tension */
            int v_ent = (int)tension_mesuree;
            int v_dec = (int)((tension_mesuree - v_ent) * 10.0f);
            sprintf(titre, "Tension: %d,%dV", v_ent, v_dec);
            donnees = historique_tension;
            valeur_max = 30.0f;
            break;
        }
        case 1: { /* Courant */
            int i_ent = (int)courant_mesure;
            int i_dec = (int)((courant_mesure - i_ent) * 100.0f);
            sprintf(titre, "Courant: %d,%02dA", i_ent, i_dec);
            donnees = historique_courant;
            valeur_max = 5.0f;
            break;
        }
        default: { /* Puissance */
            int p_ent = (int)puissance_calculee;
            int p_dec = (int)((puissance_calculee - p_ent) * 10.0f);
            sprintf(titre, "Puissance: %d,%dW", p_ent, p_dec);
            donnees = historique_puissance;
            valeur_max = 150.0f;
            break;
        }
    }

    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(titre, Font_6x8, White);

    uint8_t hauteur_graphe = 20;
    uint8_t decalage_y = 10;

    /* Tracé des axes */
    /* Axe vertical */
    for (uint8_t y = 0; y < hauteur_graphe; y++) {
        ssd1306_DrawPixel(10, decalage_y + y, White);
    }
    /* Axe horizontal */
    for (uint8_t x = 0; x < 110; x++) {
        ssd1306_DrawPixel(10 + x, decalage_y + hauteur_graphe - 1, White);
    }

    /* Tracé de la courbe */
    for (uint8_t i = 0; i < POINTS_GRAPHIQUE - 1; i++) {
        uint8_t idx_donnee = (index_donnees_graphe + i) % POINTS_GRAPHIQUE;
        uint8_t idx_suivant = (index_donnees_graphe + i + 1) % POINTS_GRAPHIQUE;

        /* Calcul des coordonnées avec normalisation */
        float ratio1 = donnees[idx_donnee] / valeur_max;
        float ratio2 = donnees[idx_suivant] / valeur_max;
        
        /* Limitation des ratios entre 0 et 1 */
        if (ratio1 > 1.0f) ratio1 = 1.0f;
        if (ratio2 > 1.0f) ratio2 = 1.0f;
        if (ratio1 < 0.0f) ratio1 = 0.0f;
        if (ratio2 < 0.0f) ratio2 = 0.0f;
        
        uint8_t y1 = decalage_y + hauteur_graphe - 1 -
                     (uint8_t)(ratio1 * (hauteur_graphe - 2));
        uint8_t y2 = decalage_y + hauteur_graphe - 1 -
                     (uint8_t)(ratio2 * (hauteur_graphe - 2));

        uint8_t x1 = 11 + (i * 110) / (POINTS_GRAPHIQUE - 1);
        uint8_t x2 = 11 + ((i + 1) * 110) / (POINTS_GRAPHIQUE - 1);

        ssd1306_Line(x1, y1, x2, y2, White);
    }

    /* Étiquettes d'échelle */
    ssd1306_SetCursor(0, decalage_y);
    if (parametre_graphique == 0) {
        ssd1306_WriteString("30", Font_6x8, White);
    } else if (parametre_graphique == 1) {
        ssd1306_WriteString("5", Font_6x8, White);
    } else {
        ssd1306_WriteString("150", Font_6x8, White);
    }

    ssd1306_SetCursor(0, decalage_y + hauteur_graphe - 8);
    ssd1306_WriteString("0", Font_6x8, White);

    ssd1306_UpdateScreen();
}

/**
  * @brief  Affichage des données du wattmètre
  */
void Afficher_Wattmetre(void)
{
    char chaine1[21] = {0};
    char chaine2[21] = {0};
    char chaine3[21] = {0};

    /* Conversion des valeurs flottantes en parties entières/décimales */
    int v_entier = (int)tension_mesuree;
    int v_decimal = (int)((tension_mesuree - v_entier) * 10.0f);
    if (v_decimal < 0) v_decimal = -v_decimal;

    int i_entier = (int)courant_mesure;
    int i_decimal = (int)((courant_mesure - i_entier) * 100.0f);
    if (i_decimal < 0) i_decimal = -i_decimal;

    int p_entier = (int)puissance_calculee;
    int p_decimal = (int)((puissance_calculee - p_entier) * 10.0f);
    if (p_decimal < 0) p_decimal = -p_decimal;

    /* Gestion de l'affichage de l'énergie */
    if (energie_cumulee < 1.0f) {
        /* Affichage en mWh si < 1 Wh */
        int e_mwh = (int)(energie_cumulee * 1000.0f);
        sprintf(chaine1, "U:%d,%dV  I:%d,%02dA", v_entier, v_decimal, i_entier, i_decimal);
        sprintf(chaine2, "P:%d,%dW E:%dmWh", p_entier, p_decimal, e_mwh);
    } else {
        /* Affichage en kWh si >= 1 Wh */
        int e_entier = (int)energie_cumulee;
        int e_decimal = (int)((energie_cumulee - e_entier) * 1000.0f);
        if (e_decimal < 0) e_decimal = -e_decimal;
        sprintf(chaine1, "U:%d,%dV  I:%d,%02dA", v_entier, v_decimal, i_entier, i_decimal);
        sprintf(chaine2, "P:%d,%dW E:%d,%03dkWh", p_entier, p_decimal, e_entier, e_decimal);
    }

    sprintf(chaine3, "ENC:%03u BTN:%s", compteur_rotation, etat_bouton ? "ACTIF" : "INACT");

    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(chaine1, Font_7x10, White);
    ssd1306_SetCursor(0, 11);
    ssd1306_WriteString(chaine2, Font_7x10, White);
    ssd1306_SetCursor(0, 22);
    ssd1306_WriteString(chaine3, Font_6x8, White);
    ssd1306_UpdateScreen();
}

/**
  * @brief  Gestionnaire d'interruption du timer TIM6
  * @note   Lecture des valeurs ADC et mise à jour des mesures
  */
void Gestionnaire_Interruption_Timer(void)
{
    /* Lecture des capteurs ADC */
    uint32_t adc_tension = Lire_Valeur_ADC(ADC_CHANNEL_3);  /* PA3 - Entrée tension */
    uint32_t adc_courant = Lire_Valeur_ADC(ADC_CHANNEL_4);  /* PA4 - Entrée courant */

    /* Conversion en grandeurs physiques */
    tension_mesuree = Convertir_ADC_Tension(adc_tension);
    courant_mesure = Convertir_ADC_Courant(adc_courant);
    puissance_calculee = Calculer_Puissance(tension_mesuree, courant_mesure);

    /* Calcul du delta temps pour intégration énergétique */
    uint32_t horodatage_actuel = HAL_GetTick();
    uint32_t intervalle = horodatage_actuel - dernier_temps;
    dernier_temps = horodatage_actuel;

    /* Mise à jour de l'énergie accumulée */
    if (intervalle > 0 && intervalle < 1000) {
        Actualiser_Energie(puissance_calculee, intervalle);
    }

    /* Actualisation des valeurs maximales */
    Actualiser_Maximums(tension_mesuree, courant_mesure, puissance_calculee);

    /* Mise à jour des données graphiques */
    Actualiser_Donnees_Graphe();

    /* Détection appui long sur bouton */
    if (etat_bouton && !appui_long_traite) {
        uint32_t duree_appui = horodatage_actuel - temps_appui_bouton;
        if (duree_appui >= 2000) {
            Executer_Action_Menu(1);
            appui_long_traite = 1;
        }
    }

    /* Retour automatique au wattmètre après délai */
    if (menu_actuel != ECRAN_WATTMETRE &&
        (horodatage_actuel - derniere_activite) > DELAI_RETOUR_MENU) {
        menu_actuel = ECRAN_WATTMETRE;
        selection_menu = 0;
        menu_modifie = 1;
    }

    /* Rafraîchissement de l'affichage */
    if (menu_modifie) {
        Afficher_Menu_Actuel();
        menu_modifie = 0;
    }
    else if (menu_actuel == ECRAN_WATTMETRE || menu_actuel == ECRAN_GRAPHIQUES) {
        Afficher_Menu_Actuel();
    }
}

/**
  * @brief  Gestionnaire d'interruption du bouton utilisateur
  * @note   Gestion avec anti-rebond logiciel
  */
void Gestionnaire_Interruption_Bouton(void)
{
    uint32_t temps_actuel = HAL_GetTick();
    uint8_t etat_brut_bouton = HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin);

    /* Anti-rebond logiciel */
    if ((temps_actuel - derniere_interruption_bouton) < 20) {
        return;
    }
    derniere_interruption_bouton = temps_actuel;

    /* Vérifier si l'état a changé */
    if (etat_brut_bouton == etat_stable_bouton) {
        return;
    }

    etat_stable_bouton = etat_brut_bouton;

    if (etat_stable_bouton && !etat_bouton) {
        /* Bouton appuyé */
        temps_appui_bouton = temps_actuel;
        appui_long_traite = 0;
        etat_bouton = 1;
    }
    else if (!etat_stable_bouton && etat_bouton) {
        /* Bouton relâché */
        uint32_t duree_appui = temps_actuel - temps_appui_bouton;

        if (!appui_long_traite && duree_appui < 2000) {
            Executer_Action_Menu(0);
        }
        etat_bouton = 0;
    }
}

/**
  * @brief  Gestionnaire d'interruption de l'encodeur rotatif
  */
void Gestionnaire_Interruption_Encodeur(void)
{
    static int8_t tampon_encodeur = 0;
    uint32_t temps_actuel = HAL_GetTick();

    /* Anti-rebond */
    if ((temps_actuel - derniere_interruption_encodeur) < 5) {
        return;
    }
    derniere_interruption_encodeur = temps_actuel;

    /* Lecture de l'état actuel */
    uint8_t nouvel_etat = HAL_GPIO_ReadPin(ROT_CHA_GPIO_Port, ROT_CHA_Pin) << 1;
    nouvel_etat |= HAL_GPIO_ReadPin(ROT_CHB_GPIO_Port, ROT_CHB_Pin);

    if (nouvel_etat != etat_encodeur) {
        int8_t sens_rotation = 0;

        /* Détection du sens horaire */
        if (((etat_encodeur == 0b00) && (nouvel_etat == 0b10)) ||
            ((etat_encodeur == 0b10) && (nouvel_etat == 0b11)) ||
            ((etat_encodeur == 0b11) && (nouvel_etat == 0b01)) ||
            ((etat_encodeur == 0b01) && (nouvel_etat == 0b00))) {
            tampon_encodeur++;
        }

        /* Détection du sens anti-horaire */
        else if (((etat_encodeur == 0b00) && (nouvel_etat == 0b01)) ||
                 ((etat_encodeur == 0b01) && (nouvel_etat == 0b11)) ||
                 ((etat_encodeur == 0b11) && (nouvel_etat == 0b10)) ||
                 ((etat_encodeur == 0b10) && (nouvel_etat == 0b00))) {
            tampon_encodeur--;
        }

        etat_encodeur = nouvel_etat;

        /* Validation après 4 transitions cohérentes */
        if (tampon_encodeur > 3) {
            compteur_rotation++;
            tampon_encodeur = 0;
            sens_rotation = 1;
        }
        else if (tampon_encodeur < -3) {
            compteur_rotation--;
            tampon_encodeur = 0;
            sens_rotation = -1;
        }

        if (sens_rotation != 0) {
            Gerer_Navigation_Menu(sens_rotation);
        }
    }
}

/**
  * @brief  Lecture d'une valeur ADC sur un canal spécifié
  * @param  canal_adc Macro du canal (ADC_CHANNEL_3 ou ADC_CHANNEL_4)
  * @retval Valeur convertie du canal
  */
uint32_t Lire_Valeur_ADC(uint32_t canal_adc)
{
    /* Réinitialisation de la sélection de canal */
    hadc.Instance->CHSELR = 0;
    
    /* Configuration du canal ADC */
    ADC_ChannelConfTypeDef config_canal = {0};
    config_canal.Channel = canal_adc;
    config_canal.Rank = ADC_RANK_CHANNEL_NUMBER;
    
    if (HAL_ADC_ConfigChannel(&hadc, &config_canal) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* Démarrage de la conversion */
    if (HAL_ADC_Start(&hadc) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* Attente de la fin de conversion */
    if (HAL_ADC_PollForConversion(&hadc, 100) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* Récupération de la valeur */
    return HAL_ADC_GetValue(&hadc);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialisation des périphériques */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_I2C1_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  
  /* Initialisation de l'écran OLED */
  ssd1306_Init();
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Wattmetre v1.0", Font_7x10, White);
  ssd1306_SetCursor(0, 11);
  ssd1306_WriteString("Mode Production", Font_7x10, White);
  ssd1306_SetCursor(0, 22);
  ssd1306_WriteString("STM32L052K6", Font_6x8, White);
  ssd1306_UpdateScreen();

  /* Initialisation des variables de mesure */
  dernier_temps = HAL_GetTick();
  derniere_activite = HAL_GetTick();
  Reinitialiser_Energie();
  Reinitialiser_Maximums();

  /* Configuration initiale du système de menu */
  menu_actuel = ECRAN_WATTMETRE;
  selection_menu = 0;
  menu_modifie = 1;

  /* Démarrage du timer pour mesures périodiques */
  HAL_TIM_Base_Start_IT(&htim6);

  /* Délai de démarrage */
  HAL_Delay(1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* Lecture de l'état initial de l'encodeur */
  etat_encodeur = HAL_GPIO_ReadPin(ROT_CHA_GPIO_Port, ROT_CHA_Pin) << 1;
  etat_encodeur |= HAL_GPIO_ReadPin(ROT_CHB_GPIO_Port, ROT_CHB_Pin);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Initialisation de l'ADC (Canaux 3 et 4)
  * @param Aucun
  * @retval Aucun
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.OversamplingMode = DISABLE;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerFrequencyMode = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_3;  /* PA3 - Entrée tension */
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configuration du second canal ADC.
  */
  sConfig.Channel = ADC_CHANNEL_4;  /* PA4 - Entrée courant */
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief Initialisation du bus I2C1 pour l'écran OLED
  * @param Aucun
  * @retval Aucun
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x0060112F;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief Initialisation du timer TIM6 pour échantillonnage
  * @param Aucun
  * @retval Aucun
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 2096;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief Configuration des broches GPIO
  * @param Aucun
  * @retval Aucun
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configuration broche bouton utilisateur (PB3) */
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /* Configuration canal A encodeur (PB5) */
  GPIO_InitStruct.Pin = ROT_CHA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ROT_CHA_GPIO_Port, &GPIO_InitStruct);

  /* Configuration canal B encodeur (PB4) */
  GPIO_InitStruct.Pin = ROT_CHB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ROT_CHB_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
