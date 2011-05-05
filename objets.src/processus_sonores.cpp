#include "processus_sonores.h"
#include <iostream>

void test_fx() {
	looplayer l;
	l.start();
	{
		reverb r(&l);
		sleep(3);
	}
	sleep(2);
	{
		reverb r(&l);
		sleep(1);
	};
	sleep(2);
}

void stress_test() {
	for(int i=0; i<2; ++i) {
		if(i % 2 == 0)
			test_fx();
		oscillateur o1(256);
		sleep(2);
		oscillateur o2(320);
		sleep(2);
		if(i % 2 == 1)
			test_fx();
		o1.frequence(384);
		o2.frequence(320*2);
		oscillateur o3(384), o4(385);
		sleep(4);
	}
}

#include <vector>
#include <stdlib.h>

double frequence(int midi_note) {
	enum { frequence_la3 = 440, midi_pitch_la3 = 69 };
	return frequence_la3 * pow(2, (midi_note - midi_pitch_la3) * 1.0 / 12);
}

// TODO: marche pas
bool recursive_dsp_loop(boite_pd* b, boite_pd* b2, std::list<boite_pd*>& deja_faits) {
	bool ok = true;
	if(b == b2) ok = false;
	for(boite_pd::iterator it = b->outlets().begin(); it != b->outlets().end() && ok; ++it) {
		if(std::find(deja_faits.begin(), deja_faits.end(), *it) == deja_faits.end()) {
			deja_faits.push_back(*it);
			ok = recursive_dsp_loop(*it, b, deja_faits);
		}
	}
	return ok;
}

/*

Un cas probable mais qu'on ne cherchera sûrement pas à éviter : les DSP loops (que ça se reboucle sur soi-même).
Sinon, il existe des cas où l'on se branche


*/
void more_stressful_test() {
	using namespace std;
	srand(time(NULL));
	vector<processus_sonore*> procs;
	unsigned int n = 10;
	
	// Ajout de n éléments au pif
	for(unsigned int i = 0; i < n; ++i) {
		unsigned int choix;
		do {
			choix = rand() % 3;
		} while(procs.size() == 0 && choix == 2);
		if(choix == 0) {
			procs.push_back(new oscillateur(frequence(69 + rand() % 12)));
		} else if(choix == 1) {
			looplayer* p = new looplayer; 
			procs.push_back(p);
			p->start();
		} else/* if(choix == 2)*/ {
			unsigned int source_1;
			unsigned int source_2;
			source* s = 0;
			do {
				source_1 = rand() % procs.size();
				source_2 = rand() % (procs.size() + 1);
				if(source_2 != procs.size())
					s = dynamic_cast<source*>(procs[source_2]);
				else
					s = 0;
			} while(source_2 == source_1 || s);
			processus_sonore* p1 = procs[source_1];
			boite_pd* p2 = procs[source_2];
			if(source_2 == procs.size())
				p2 = &master_pd::instance();
			
			processus_sonore* newp;
			
			// Choix du fx
			if(rand() % 2 == 0)
				newp = new simple_delay(p1, p2);
			else
				newp = new reverb(p1, p2);
			
			// Ajout si pas de boucle
			std::list<boite_pd*> df;
			if(recursive_dsp_loop(newp, 0, df))
				procs.push_back(newp);
			else {
				std::cout << "DSP loop" << std::endl;
				delete newp;
			}
		}
		sleep(1 + rand() % 3);
	}
	
	// Effacement
	for(vector<processus_sonore*>::iterator it = procs.begin(); it != procs.end(); ++it)
		delete *it;
}

void fx_test() {
	looplayer l;
	l.start();
	sleep(3);
	simple_delay *d1 = new simple_delay(&l);
	sleep(3);
	d1->delai(400);
	d1->wet(0.5);
	sleep(3);
	reverb* r1 = new reverb(d1);
	sleep(3);
	delete d1;
	sleep(5);
	delete r1;
}

// int main() {
// 	master_pd::instance();
// 	// stress_test();
// 	// more_stressful_test();
// 	// fx_test();
// 	line_in line;
// 	simple_delay d1(&line);
// 	d1.delai(350); d1.wet(0.3); d1.feedback(0.4);
// 	sleep(4);
// 	reverb* r1 = new reverb(&d1);
// 	sleep(4);
// 	delete r1;
// 	sleep(20);
// 	return 0;
// }