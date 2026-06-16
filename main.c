#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "monopoly.h" // NEMENIT !!!

// vrati cislo pola na ktorom je property
int cisloPola(PROPERTY *prop) {
    for (int i = 0; i < NUM_SPACES; i++) {
        if (game_board[i].property == prop)
            return i + 1;
    }
    return -1;
}

// vrati pointer na hraca ktory vlastni property
PLAYER *vlastnik(PROPERTY *prop, PLAYER *hraci, int pocet_hracov) {
    for (int i = 0; i < pocet_hracov; i++) {
        for (int j = 0; j < hraci[i].num_properties; j++) {
            if (hraci[i].owned_properties[j] == prop)
                return &hraci[i];
        }
    }
    return NULL;
}

// vrati 1 ak ma monopol a 0 ak nie
int monopol(PLAYER *hrac, COLOR farba) {
    int pocet = 0;
    for (int i = 0; i < hrac->num_properties; i++) {
        if (hrac->owned_properties[i]->color == farba)
            pocet++;
    }
    return pocet == 2;
}

// vypis herneho planu
void vypis_plan(PLAYER *hraci, int pocet_hracov) {
    printf("Game board:\n");
    for (int i = 0; i < NUM_SPACES; i++) {
        SPACE *pole = &game_board[i];
        char cislo[6];
        snprintf(cislo, sizeof(cislo), "%d.", i + 1);

        if (pole->type != Property) {
            printf("%-5s%s\n", cislo, space_types[pole->type]);
            continue;
        }

        PROPERTY *prop = pole->property;
        PLAYER *own = vlastnik(prop, hraci, pocet_hracov);

        if (own == NULL) {
            printf("%-5s%-18s%-3d %s\n",
                   cislo, prop->name, prop->price, property_colors[prop->color]);
        } else {
            printf("%-5s%-18s%-3d %-10s P%d %s\n",
                   cislo, prop->name, prop->price,
                   property_colors[prop->color],
                   own->number,
                   monopol(own, prop->color) ? "yes" : "no");
        }
    }
}

// vypis hracov
void vypis_hracov(PLAYER *hraci, int pocet_hracov) {
    printf("Players:\n");
    for (int i = 0; i < pocet_hracov; i++) {
        PLAYER *p = &hraci[i];
        printf("%d. S: %d, C: %d, JP: %d, IJ: %s\n",
               p->number, p->space_number, p->cash,
               p->num_jail_pass, p->is_in_jail ? "yes" : "no");

        for (int j = 0; j < p->num_properties; j++) {
            PROPERTY *prop = p->owned_properties[j];
            printf("  %s %d %s S%d\n",
                   prop->name, prop->price,
                   property_colors[prop->color],
                   cisloPola(prop));
        }
    }
}

// vypis stavu hry
void vypis_stav_hry(PLAYER *hraci, int pocet_hracov, int bankrot_index) {
    vypis_hracov(hraci, pocet_hracov);
    vypis_plan(hraci, pocet_hracov);

    if (bankrot_index == -1) {
        printf("WINNER: -\n\n");
        return;
    }

    int max_skore = -1;
    int vitaz = -1;
    int zhoda = 0;

    for (int i = 0; i < pocet_hracov; i++) {
        if (i == bankrot_index) continue;

        int skore = hraci[i].cash;
        for (int j = 0; j < hraci[i].num_properties; j++)
            skore += hraci[i].owned_properties[j]->price;

        if (skore > max_skore) {
            max_skore = skore;
            vitaz = i;
            zhoda = 1;
        } else if (skore == max_skore) {
            zhoda++;
        }
    }

    if (zhoda == 1)
        printf("WINNER: P%d\n", hraci[vitaz].number);
    else
        printf("WINNER: ?\n");

    printf("\n");
}

int main(int argc, char *argv[]) {
    int pocet_hracov = 2;
    int flag_s = 0, flag_p = 0, flag_g = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0)
            pocet_hracov = atoi(argv[++i]);
        else if (strcmp(argv[i], "-s") == 0)
            flag_s = 1;
        else if (strcmp(argv[i], "-p") == 0)
            flag_p = 1;
        else if (strcmp(argv[i], "-g") == 0)
            flag_g = 1;
    }

    int start_cash[] = {0, 0, 20, 18, 16};
    PLAYER hraci[MAX_PLAYERS];

    for (int i = 0; i < pocet_hracov; i++) {
        hraci[i].number = i + 1;
        hraci[i].space_number = 1;
        hraci[i].cash = start_cash[pocet_hracov];
        hraci[i].num_jail_pass = 0;
        hraci[i].is_in_jail = 0;
        hraci[i].num_properties = 0;
    }

    vypis_stav_hry(hraci, pocet_hracov, -1);

    int tah = 0;
    int na_tahu = 0;
    int bankrot = -1;

    while (1) {
        PLAYER *p = &hraci[na_tahu];
        tah++;

        int hod;
        if (scanf("%d", &hod) != 1)
            break;

        printf("\nR: %d\n", hod);
        printf("Turn: %d\n", tah);
        printf("Player on turn: P%d\n\n", p->number);

        // vazenie - zaplat pokutu pred pohybom
        if (p->is_in_jail) {
            if (p->cash < 1) { bankrot = na_tahu; break; }
            p->cash--;
            p->is_in_jail = 0;
        }

        // pohyb
        p->space_number += hod;

        // presiel hrac cez koniec planu (24 poli)?
        int presiel_start = 0;
        if (p->space_number > NUM_SPACES) {
            p->space_number -= NUM_SPACES;
            presiel_start = 1;
        }

        // odmena za prechod cez start alebo pristatie na start
        if (presiel_start || p->space_number == 1)
            p->cash += 2;

        // akcia podla pola
        SPACE *pole = &game_board[p->space_number - 1];

        if (pole->type == Property) {
            PROPERTY *prop = pole->property;
            PLAYER *own = vlastnik(prop, hraci, pocet_hracov);

            if (own == NULL) {
                // kup nehnutelnost
                if (p->cash < prop->price) { bankrot = na_tahu; break; }
                p->cash -= prop->price;
                p->owned_properties[p->num_properties++] = prop;
            } else if (own != p) {
                // zaplat najomne
                int najomne = monopol(own, prop->color) ? prop->price * 2 : prop->price;
                if (p->cash < najomne) { bankrot = na_tahu; break; }
                p->cash -= najomne;
                own->cash += najomne;
            }

        } else if (pole->type == Jail_pass) {
            p->num_jail_pass++;

        } else if (pole->type == Go_to_jail) {
            if (p->num_jail_pass > 0) {
                p->num_jail_pass--;
            } else {
                for (int i = 0; i < NUM_SPACES; i++) {
                    if (game_board[i].type == In_jail) {
                        p->space_number = i + 1;
                        break;
                    }
                }
                p->is_in_jail = 1;
            }
        }

        if (flag_s) vypis_plan(hraci, pocet_hracov);
        if (flag_p) vypis_hracov(hraci, pocet_hracov);
        if (flag_g) vypis_stav_hry(hraci, pocet_hracov, -1);

        na_tahu = (na_tahu + 1) % pocet_hracov;
    }

    vypis_stav_hry(hraci, pocet_hracov, bankrot);
    return 0;
}