/*
 * SYSTEME DE GESTION ACADEMIQUE
 * Langage : C
 * Notions : structures, pointeurs, listes chainees, arbres binaires,
 *           tableaux, tri et fichiers.
 *
 * Compilation : gcc gestion_academique.c -o gestion_academique
 * Execution  : ./gestion_academique
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NB_MATIERES 3
#define TAILLE_NOM 40
#define FICHIER "etudiants.txt"

const char *matieres[NB_MATIERES] = {
    "Algorithmique", "Mathematiques", "Programmation"
};

typedef struct {
    int id;
    char nom[TAILLE_NOM];
    char prenom[TAILLE_NOM];
    float notes[NB_MATIERES];
} Etudiant;

typedef struct Cellule {
    Etudiant info;
    struct Cellule *suivant;
} Cellule;

typedef struct Noeud {
    Etudiant *etudiant;
    struct Noeud *gauche;
    struct Noeud *droite;
} Noeud;

/* ---------- Saisie securisee ---------- */

void lire_texte(const char *message, char *destination, int taille) {
    printf("%s", message);
    if (fgets(destination, taille, stdin) != NULL) {
        destination[strcspn(destination, "\n")] = '\0';
    }
}

int lire_entier(const char *message) {
    char ligne[100];
    int valeur;
    while (1) {
        lire_texte(message, ligne, sizeof(ligne));
        if (sscanf(ligne, "%d", &valeur) == 1) return valeur;
        printf("Veuillez entrer un nombre entier.\n");
    }
}

float lire_note(const char *message) {
    char ligne[100];
    float note;
    while (1) {
        lire_texte(message, ligne, sizeof(ligne));
        if (sscanf(ligne, "%f", &note) == 1 && note >= 0 && note <= 20)
            return note;
        printf("Note invalide. Entrez une valeur entre 0 et 20.\n");
    }
}

/* ---------- Liste chainee ---------- */

Cellule *chercher_liste(Cellule *liste, int id) {
    while (liste != NULL) {
        if (liste->info.id == id) return liste;
        liste = liste->suivant;
    }
    return NULL;
}

void ajouter_en_tete(Cellule **liste, Etudiant e) {
    Cellule *nouveau = malloc(sizeof(Cellule));
    if (nouveau == NULL) {
        printf("Erreur : memoire insuffisante.\n");
        return;
    }
    nouveau->info = e;
    nouveau->suivant = *liste;
    *liste = nouveau;
}

void ajouter_etudiant(Cellule **liste) {
    Etudiant e;

    e.id = lire_entier("Numero de l'etudiant : ");
    if (chercher_liste(*liste, e.id) != NULL) {
        printf("Ce numero existe deja.\n");
        return;
    }

    lire_texte("Nom : ", e.nom, sizeof(e.nom));
    lire_texte("Prenom : ", e.prenom, sizeof(e.prenom));

    for (int i = 0; i < NB_MATIERES; i++) {
        char message[100];
        snprintf(message, sizeof(message), "Note en %s (/20) : ", matieres[i]);
        e.notes[i] = lire_note(message);
    }

    ajouter_en_tete(liste, e);
    printf("Etudiant ajoute avec succes.\n");
}

float calculer_moyenne(const Etudiant *e) {
    float somme = 0;
    for (int i = 0; i < NB_MATIERES; i++) somme += e->notes[i];
    return somme / NB_MATIERES;
}

void afficher_un_etudiant(const Etudiant *e) {
    printf("\nNumero : %d | %s %s\n", e->id, e->prenom, e->nom);
    for (int i = 0; i < NB_MATIERES; i++)
        printf("  %-16s : %5.2f/20\n", matieres[i], e->notes[i]);
    printf("  Moyenne          : %5.2f/20\n", calculer_moyenne(e));
}

void afficher_tous(Cellule *liste) {
    if (liste == NULL) {
        printf("Aucun etudiant enregistre.\n");
        return;
    }
    printf("\n========== LISTE DES ETUDIANTS ==========\n");
    while (liste != NULL) {
        afficher_un_etudiant(&liste->info);
        liste = liste->suivant;
    }
}

void modifier_etudiant(Cellule *liste) {
    int id = lire_entier("Numero de l'etudiant a modifier : ");
    Cellule *c = chercher_liste(liste, id);

    if (c == NULL) {
        printf("Etudiant introuvable.\n");
        return;
    }

    printf("Modification des informations de %s %s\n",
           c->info.prenom, c->info.nom);
    lire_texte("Nouveau nom : ", c->info.nom, sizeof(c->info.nom));
    lire_texte("Nouveau prenom : ", c->info.prenom, sizeof(c->info.prenom));

    for (int i = 0; i < NB_MATIERES; i++) {
        char message[100];
        snprintf(message, sizeof(message), "Nouvelle note en %s : ", matieres[i]);
        c->info.notes[i] = lire_note(message);
    }
    printf("Etudiant modifie.\n");
}

void supprimer_etudiant(Cellule **liste) {
    int id = lire_entier("Numero de l'etudiant a supprimer : ");
    Cellule *courant = *liste, *precedent = NULL;

    while (courant != NULL && courant->info.id != id) {
        precedent = courant;
        courant = courant->suivant;
    }

    if (courant == NULL) {
        printf("Etudiant introuvable.\n");
        return;
    }

    if (precedent == NULL) *liste = courant->suivant;
    else precedent->suivant = courant->suivant;

    free(courant);
    printf("Etudiant supprime.\n");
}

/* ---------- Arbre binaire de recherche ---------- */

Noeud *inserer_arbre(Noeud *racine, Etudiant *e) {
    if (racine == NULL) {
        Noeud *n = malloc(sizeof(Noeud));
        if (n == NULL) return NULL;
        n->etudiant = e;
        n->gauche = n->droite = NULL;
        return n;
    }

    if (e->id < racine->etudiant->id)
        racine->gauche = inserer_arbre(racine->gauche, e);
    else if (e->id > racine->etudiant->id)
        racine->droite = inserer_arbre(racine->droite, e);

    return racine;
}

Noeud *construire_arbre(Cellule *liste) {
    Noeud *racine = NULL;
    while (liste != NULL) {
        racine = inserer_arbre(racine, &liste->info);
        liste = liste->suivant;
    }
    return racine;
}

Etudiant *chercher_arbre(Noeud *racine, int id) {
    while (racine != NULL) {
        if (id == racine->etudiant->id) return racine->etudiant;
        if (id < racine->etudiant->id) racine = racine->gauche;
        else racine = racine->droite;
    }
    return NULL;
}

void liberer_arbre(Noeud *racine) {
    if (racine == NULL) return;
    liberer_arbre(racine->gauche);
    liberer_arbre(racine->droite);
    free(racine);
}

void rechercher_etudiant(Cellule *liste) {
    int id = lire_entier("Numero a rechercher : ");
    Noeud *index = construire_arbre(liste);
    Etudiant *e = chercher_arbre(index, id);

    if (e == NULL) printf("Etudiant introuvable.\n");
    else afficher_un_etudiant(e);

    liberer_arbre(index);
}

/* ---------- Classement avec tableau et tri ---------- */

void afficher_classement(Cellule *liste) {
    int n = 0;
    for (Cellule *c = liste; c != NULL; c = c->suivant) n++;

    if (n == 0) {
        printf("Aucun etudiant a classer.\n");
        return;
    }

    Etudiant **tab = malloc(n * sizeof(Etudiant *));
    if (tab == NULL) {
        printf("Erreur d'allocation memoire.\n");
        return;
    }

    int i = 0;
    for (Cellule *c = liste; c != NULL; c = c->suivant)
        tab[i++] = &c->info;

    /* Tri par moyenne decroissante (tri a bulles). */
    for (i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (calculer_moyenne(tab[j]) < calculer_moyenne(tab[j + 1])) {
                Etudiant *temp = tab[j];
                tab[j] = tab[j + 1];
                tab[j + 1] = temp;
            }
        }
    }

    printf("\n============= CLASSEMENT =============\n");
    for (i = 0; i < n; i++) {
        printf("%d. %-15s %-15s | Moyenne : %.2f/20\n",
               i + 1, tab[i]->prenom, tab[i]->nom,
               calculer_moyenne(tab[i]));
    }
    free(tab);
}

/* ---------- Sauvegarde et chargement ---------- */

void sauvegarder(Cellule *liste) {
    FILE *f = fopen(FICHIER, "w");
    if (f == NULL) {
        printf("Impossible d'ouvrir le fichier de sauvegarde.\n");
        return;
    }

    for (Cellule *c = liste; c != NULL; c = c->suivant) {
        fprintf(f, "%d|%s|%s|%.2f|%.2f|%.2f\n",
                c->info.id, c->info.nom, c->info.prenom,
                c->info.notes[0], c->info.notes[1], c->info.notes[2]);
    }

    fclose(f);
    printf("Donnees sauvegardees dans %s.\n", FICHIER);
}

void charger(Cellule **liste) {
    FILE *f = fopen(FICHIER, "r");
    if (f == NULL) {
        printf("Aucune sauvegarde precedente trouvee.\n");
        return;
    }

    /* On remplace la liste actuelle par le contenu du fichier. */
    while (*liste != NULL) {
        Cellule *temp = *liste;
        *liste = (*liste)->suivant;
        free(temp);
    }

    char ligne[256];
    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        Etudiant e;
        if (sscanf(ligne, "%d|%39[^|]|%39[^|]|%f|%f|%f",
                   &e.id, e.nom, e.prenom,
                   &e.notes[0], &e.notes[1], &e.notes[2]) == 6) {
            if (chercher_liste(*liste, e.id) == NULL)
                ajouter_en_tete(liste, e);
        }
    }

    fclose(f);
    printf("Donnees chargees depuis %s.\n", FICHIER);
}

void liberer_liste(Cellule **liste) {
    while (*liste != NULL) {
        Cellule *temp = *liste;
        *liste = (*liste)->suivant;
        free(temp);
    }
}

/* ---------- Programme principal ---------- */

int main(void) {
    Cellule *liste = NULL;
    int choix;

    charger(&liste);

    do {
        printf("\n========================================\n");
        printf("      SYSTEME DE GESTION ACADEMIQUE\n");
        printf("========================================\n");
        printf("1. Ajouter un etudiant\n");
        printf("2. Afficher tous les etudiants\n");
        printf("3. Rechercher un etudiant (arbre binaire)\n");
        printf("4. Modifier un etudiant\n");
        printf("5. Supprimer un etudiant\n");
        printf("6. Afficher le classement\n");
        printf("7. Sauvegarder les donnees\n");
        printf("0. Quitter\n");
        printf("----------------------------------------\n");

        choix = lire_entier("Votre choix : ");

        switch (choix) {
            case 1: ajouter_etudiant(&liste); break;
            case 2: afficher_tous(liste); break;
            case 3: rechercher_etudiant(liste); break;
            case 4: modifier_etudiant(liste); break;
            case 5: supprimer_etudiant(&liste); break;
            case 6: afficher_classement(liste); break;
            case 7: sauvegarder(liste); break;
            case 0:
                sauvegarder(liste);
                printf("Fermeture du programme. Au revoir !\n");
                break;
            default:
                printf("Choix invalide.\n");
        }
    } while (choix != 0);

    liberer_liste(&liste);
    return 0;
}
