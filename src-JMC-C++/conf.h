/*
 * conf.h
 *
 *  Created on: 13 déc. 2011
 *      Author: ppudlo
 */

#ifndef CONF_H_
#define CONF_H_
#include "header.h"
#include "matrices.h"
#include "mesutils.h"
#include "particleset.h"

// Ecriture de l'entete du fichier confidence.txt contenant les résultats
void ecrientete(int nrec, int ntest,int nseld, int nselr,int nlogreg,string shist,string smut, bool AFD);

// Appelle l'analyse factorielle discriminante et remplace les summary stats par les composantes des jeux simulés
// sur les axes discriminants
float* transfAFD(int nrec,int nsel, int p);

// Interprête la ligne de paramètres de l'option "confiance dans le choix du scénario"
// et lance les calculs correspondants
void doconf(string opt, int seed);


#endif /* CONF_H_ */
