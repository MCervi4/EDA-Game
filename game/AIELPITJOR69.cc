#include "Player.hh"
#include <vector>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME ELPITJOR69


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
  //def vectors
  typedef vector<int> vint;
  typedef vector<bool> vbool;
  typedef vector<Pos> vpos;
  //def matrius
  typedef vector<vint> MI;
  typedef vector<vbool> MB;
  typedef vector<vpos> MP;

  //retorna true si esta fora del mapa
  bool fora_del_mapa(Pos pos) {
      int i=pos.i;
      int j=pos.j;
      if (i>=0 and i<60 and j>=0 and j<60) return false;
      return true;
  }
  
   bool cell_available(int obj, Pos pos, bool recarg ) { //obj=0 warrior, type=1 car
      if (not fora_del_mapa(pos)) {
        Cell c = cell(pos);
        if (c.type==Desert or c.type==Road) return true;
        else if (obj==0 and (c.type==City or ( recarg and c.type==Water ) ) ) return true;
        else if (obj==1 and recarg and c.type==Station) return true;
      }
      return false;
   }
   
   bool only_road(Pos pos, bool recarg ) {
      if (not fora_del_mapa(pos)) {
        Cell c = cell(pos);
        if (c.type==Road) return true;
        else if (recarg and c.type==Station) return true;
      }
      return false;
   }
  
    //retorna true si hi ha una unitat meva
    bool meva(Pos pos) {
        if (not fora_del_mapa(pos)) {
            Cell c=cell(pos);
            if (c.id!=-1) if ( unit(c.id).player==me() ) return true;
        }
        return false;
    }
    
    //retorna true si hi ha un car
    bool car(Pos pos) {
        if (not fora_del_mapa(pos)) {
            Cell c=cell(pos);
            if (c.id!=-1) if ( unit(c.id).type==Car ) return true;
        }
        return false;
    }
    
    //retorna true si la posicio te un car adjacent diferent a la id 
    bool car_adj(Pos pos, int idd) {
        if (not fora_del_mapa(pos)) {
            for (int k=0; k<8; ++k) {
                if (not fora_del_mapa(pos+Dir(k))) {
                    Cell c=cell(pos+Dir(k));
                    if (c.id!=-1 and c.id!=idd) if ( unit(c.id).type==Car ) return true;
                }
            }
        }
        return false;
    }
    
    //retorna true si hi ha un warrior
    bool warrior(Pos pos) {
        if (not fora_del_mapa(pos)) {
            Cell c=cell(pos);
            if (c.id!=-1) if ( unit(c.id).type==Warrior ) return true;
        }
        return false;
    }
    bool warrior_fora_ciutat(Pos pos) {
        if (not fora_del_mapa(pos)) {
            Cell c=cell(pos);
            if (c.id!=-1) if ( unit(c.id).type==Warrior and c.type!=City) return true;
        }
        return false;
    }
    
    bool warrior_no_meu_ajd(Pos pos) {
        if (not fora_del_mapa(pos)) {
            for (int k=0; k<8; ++k) {
                if (not fora_del_mapa(pos+Dir(k))) {
                    Cell c=cell(pos+Dir(k));
                    if (c.id!=-1) if ( unit(c.id).type==Warrior and warrior_fora_ciutat(pos+Dir(k)) and not meva(pos+Dir(k))) return true;
                }
            }
        }
        return false;
    }
  
    void explorar_w(Pos pos_expl, queue<Pos>& cua, Pos anterior, MI& dist, MB& explorat, MP& cami){
    if (not cell_available(0, pos_expl, false)) return;
    int i=pos_expl.i;
    int j=pos_expl.j;
    /*bool kamikaze=false;
    if ( cell(pos_expl).id!=-1 and not warrior(pos_expl) ) kamikaze=true;*/
    if (not explorat[i][j] and cell(pos_expl).id == -1/* and not kamikaze*/){
        explorat[i][j]=true;
        dist[i][j]=dist[anterior.i][anterior.j]+1;
        cami[i][j]=anterior;
        Pos posicio;
        posicio.i=i;
        posicio.j=j;
        cua.push(posicio);
    }
  }
  
  int buscar_desti_w(Pos inici, Pos& desti, MI& dist, MB& explorat, MP& cami){
    queue<Pos> cua;
    cua.push(inici);
    while (not cua.empty()){
      Pos act=cua.front();
      cua.pop();
      int i=act.i;
      int j=act.j;
      if (round()>50 and cell(act).type == City){
        desti=act;
        return dist[i][j];
      }
      else if ( round()<=50 and cell(act).type == City and unit(cell(inici).id).id%2==0 ) {
            desti=act;
            return dist[i][j];
      }
      else {
        if ( round()<=50 and cell(act).type == City and cell(act).owner!=me() ){
            desti=act;
            return dist[i][j];
        }
      }
      
      for (int k=0; k<8; ++k) explorar_w(act+Dir(k), cua, act, dist, explorat, cami);
    }
    return -1;
  }
  
  void explorar_water(Pos pos_expl, queue<Pos>& cua, Pos anterior, MI& dist, MB& explorat, MP& cami) {
    if (not cell_available(0, pos_expl, true)) return;
    int i=pos_expl.i;
    int j=pos_expl.j;
    /*bool kamikaze=false;
    if ( cell(pos_expl).id!=-1 and not warrior(pos_expl) ) kamikaze=true;*/
    if (not explorat[i][j] and not meva(pos_expl)/* and not kamikaze */) {
        explorat[i][j]=true;
        dist[i][j]=dist[anterior.i][anterior.j]+1;
        cami[i][j]=anterior;
        Pos posicio;
        posicio.i=i;
        posicio.j=j;
        cua.push(posicio);
    }
  }
  
  int bfs_water(Pos inici, Pos& desti, MI& dist, MB& explorat, MP& cami){
    queue<Pos> cua;
    cua.push(inici);
    while (not cua.empty()){
      Pos act=cua.front();
      cua.pop();
      int i=act.i;
      int j=act.j;
      if(cell(act).type == Water) {
          desti=act;
          return dist[i][j];
          
    }
      
      for (int k=0; k<8; ++k){
          explorar_water(act+Dir(k), cua, act, dist, explorat, cami);
      }
    }
    return -1;
  }
  
  void explorar_carretera(Pos pos_expl, queue<Pos>& cua, Pos anterior, MI& dist, MB& explorat, MP& cami) {
    if (not cell_available(1, pos_expl, false)) return;
    int i=pos_expl.i;
    int j=pos_expl.j;
    bool kamikaze=false;
    if ( cell(pos_expl).id!=-1 and not warrior(pos_expl) ) kamikaze=true;
    if (not explorat[i][j] and not meva(pos_expl) and not kamikaze ) {
        explorat[i][j]=true;
        dist[i][j]=dist[anterior.i][anterior.j]+1;
        cami[i][j]=anterior;
        Pos posicio;
        posicio.i=i;
        posicio.j=j;
        cua.push(posicio);
    }
  }
  
  int bfs_carretera(Pos inici, Pos& desti, MI& dist, MB& explorat, MP& cami){
    queue<Pos> cua;
    cua.push(inici);
    while (not cua.empty()){
      Pos act=cua.front();
      cua.pop();
      int i=act.i;
      int j=act.j;
      if(cell(act).type == Road) {
          desti=act;
          return dist[i][j];
          
    }
      
      for (int k=0; k<8; ++k){
          explorar_carretera(act+Dir(k), cua, act, dist, explorat, cami);
      }
    }
    return -1;
  }
  
  void explorar_c(Pos pos_expl, queue<Pos>& cua, Pos anterior, MI& dist, MB& explorat, MP& cami, bool fuel){
    if (not only_road(pos_expl, fuel)) return;
    int i=pos_expl.i;
    int j=pos_expl.j;
    /*bool kamikaze=false;
    if ( cell(pos_expl).id!=-1 and not warrior(pos_expl) ) kamikaze=true;*/
    if (not explorat[i][j] and not meva(pos_expl)/* and not kamikaze */) {
        explorat[i][j]=true;
        dist[i][j]=dist[anterior.i][anterior.j]+1;
        cami[i][j]=anterior;
        Pos posicio;
        posicio.i=i;
        posicio.j=j;
        cua.push(posicio);
    }
  }

  int buscar_desti_c(Pos inici, Pos& desti, MI& dist, MB& explorat, MP& cami, bool fuel){
    queue<Pos> cua;
    cua.push(inici);
    while (not cua.empty()){
      Pos act=cua.front();
      cua.pop();
      int i=act.i;
      int j=act.j;
      if (fuel) {
        if(cell(act).type == Station){
            desti=act;
            return dist[i][j];
        }
      }
      else {
          if ( (warrior_fora_ciutat(act) and not meva(act) ) or warrior_no_meu_ajd(act) ) {
          desti=act;
          return dist[i][j];
        }
        
      }
      for (int k=0; k<8; ++k){
          explorar_c(act+Dir(k), cua, act, dist, explorat, cami, fuel);
      }
    }
    return -1;
  }
  
  void on_anar(Pos inici, Pos adj, int & d) {
      for (int k=0; k<8; ++k) {
          if (inici+Dir(k)==adj) {
              d=k;
              return;
          }
      }
      d=-1;
      return;
  }
  
  bool guanyo(Pos vs, int id) { 
      Cell v=cell(vs);
      if ( unit(id).food >= unit(v.id).food /*and unit(id).water > unit(v.id).water*/) return true;
      else return false;
      //return true;
  }
  
  void kill_adj(Pos pos, int id, bool& ha_atacat) {
      for (int k=0; k<8; ++k) {
          if ( warrior(pos+Dir(k)) and not meva(pos+Dir(k)) ) {
              if ( unit(id).type==Warrior and guanyo(pos+Dir(k), id) ) {
                  ha_atacat=true;
                  command (id, Dir(k));
              }
              else if (unit(id).type==Car and warrior_fora_ciutat(pos+Dir(k))) {
                  if (not car_adj(pos+Dir(k), id)) {
                      ha_atacat=true;
                      command (id, Dir(k));
                  }
              }
          }
      }
  }
  
  void move_warriors() {
      if (round()% 4 != me()) return;
      vector<int> w=warriors(me());
      int n=w.size();
      for (int y=0; y<n; ++y) {
          int id=w[y];
          Pos inici=unit(id).pos;
          Pos desti_wat;
          int awa=unit(id).water;
          bool water=false;
          MI dist_wat( 60, vector<int>(60, -1) );
          MB explorat_wat( 60, vector<bool>(60, false) );
          MP cami_wat( 60, vector<Pos>(60) );
          dist_wat[inici.i][inici.j]=0;
          explorat_wat[inici.i][inici.j]=true;
          int wotah=bfs_water(inici, desti_wat, dist_wat, explorat_wat, cami_wat);
          if (wotah+1>=awa) water=true;
          if (water) {
              int i=desti_wat.i;
              int j=desti_wat.j;
              while(dist_wat[desti_wat.i][desti_wat.j]>=2) desti_wat=cami_wat[desti_wat.i][desti_wat.j];
              int d;
              on_anar(inici, desti_wat, d);
              if (d!=-1) if (not car(inici+Dir(d)) and not car_adj(inici+Dir(d), -2)) command(id, Dir(d));
          }
          else {
            bool ha_atacat=false;
            kill_adj(inici, id, ha_atacat);
            if (not ha_atacat and not water) {
                MI dist( 60, vector<int>(60, -1) );
                MB explorat( 60, vector<bool>(60, false) );
                MP cami( 60, vector<Pos>(60) );
                dist[inici.i][inici.j]=0;
                explorat[inici.i][inici.j]=true;
                Pos desti;
                int k=buscar_desti_w(inici, desti, dist, explorat, cami);
                if (k!=-1) {
                    int i=desti.i;
                    int j=desti.j;
                    while(dist[desti.i][desti.j]>=2) desti=cami[desti.i][desti.j];
                    int d;
                    on_anar(inici, desti, d);
                    if (d!=-1) if (not car(inici+Dir(d)) and not car_adj(inici+Dir(d), -2)) command(id, Dir(d));
                }
            }
          }
        }
  }
  
  void move_cars() {
      vector<int> w=cars(me());
      int n=w.size();
      for (int y=0; y<n; ++y) {
          int id=w[y];
          Pos inici=unit(id).pos;
          //Cell c = cell(inici);
          //if (c.type!=Road and round()% 4 != me()) return;
          if (can_move(id)) {
            bool ha_atacat=false;
            kill_adj(inici, id, ha_atacat);
            if (not ha_atacat) {
                MI dist( 60, vector<int>(60, -1) );
                MB explorat( 60, vector<bool>(60, false) );
                MP cami( 60, vector<Pos>(60) );
                MI dist2( 60, vector<int>(60, -1) );
                MB explorat2( 60, vector<bool>(60, false) );
                MP cami2( 60, vector<Pos>(60) );
                int gazo=unit(id).food;
                bool fuel=false;
                if (gazo<20) fuel=true;
                dist[inici.i][inici.j]=0;
                explorat[inici.i][inici.j]=true;
                Pos desti;
                dist2[inici.i][inici.j]=0;
                explorat2[inici.i][inici.j]=true;
                Pos desti2;
                //bool alguien = false;
                if (cell(inici).type == Desert) {
                    /*for (int k = 0; k < 8 and not alguien; ++k) {
                        Pos post = inici + Dir(k);
                        if (warrior(post)) alguien = true;
                    }
                }
                if (not alguien) {*/
                    int l=bfs_carretera(inici, desti2, dist2, explorat2, cami2); //cap a la road
                }
                int k=buscar_desti_c(inici, desti, dist, explorat, cami, fuel); //cap a warrior

                if (cell(inici).type == Desert) {
                    int i=desti2.i;
                    int j=desti2.j;
                    while(dist2[desti2.i][desti2.j]>=2) desti2=cami2[desti2.i][desti2.j];
                    int d;
                    on_anar(inici, desti2, d);
                    if (d!=-1) if (not car(inici+Dir(d)) and not car_adj(inici+Dir(d), id)) command(id, Dir(d));
                }
                
                else if (k!=-1) {
                    int i=desti.i;
                    int j=desti.j;
                    while(dist[desti.i][desti.j]>=2) desti=cami[desti.i][desti.j];
                    int d;
                    on_anar(inici, desti, d);
                    if (d!=-1) if (not car(inici+Dir(d)) and not car_adj(inici+Dir(d), id)) command(id, Dir(d));
                }
            }
          }
      }
  }
  
  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
      move_warriors();
      move_cars();
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
