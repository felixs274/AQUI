<template>

  <v-container class="my-4">

    <v-layout row wrap justify-space-around class="mx-1">

      <v-flex xs12 sm12 class="pa-4 text-center"><h1 class="font-weight-light">AQUI</h1></v-flex>

      <v-flex xs6 sm3>
        <v-card
          class="py-5 ma-3 text-center"
          elevation="5"
        >
          <v-list-item three-line>
            <v-list-item-content>
              <v-list-item-title class="text-h7 mt-2">
                Aquarium
              </v-list-item-title>
              <v-list-item-subtitle>
                          
                <v-chip v-if="values.s1 == false" label class="mt-2" color="green" text-color="white">Gut</v-chip>
                <v-chip v-if="values.s1 == true" label class="mt-2" color="orange" text-color="white">Niedrig</v-chip>

              </v-list-item-subtitle>
            </v-list-item-content>

          </v-list-item>

          <v-card-actions>
          </v-card-actions>
        </v-card>        
      </v-flex>
      <v-flex xs6 sm3>
        <v-card
          class="py-5 ma-3 text-center"
          elevation="5"
        >
          <v-list-item three-line>
            <v-list-item-content>
              <v-list-item-title class="text-h7 mt-2">
                Technik
              </v-list-item-title>
              <v-list-item-subtitle>
                          
                <v-chip v-if="values.s3 == false" label class="mt-2" color="red" text-color="white">Alarm</v-chip>
                <v-chip v-else-if="values.s2 == false" label class="mt-2" color="green" text-color="white">Gut</v-chip>
                <v-chip v-else-if="values.s2 == true" label class="mt-2" color="orange" text-color="white">Niedrig</v-chip>

              </v-list-item-subtitle>
            </v-list-item-content>

          </v-list-item>

          <v-card-actions>
          </v-card-actions>
        </v-card>        
      </v-flex>
      <v-flex xs6 sm3>
        <v-card
          class="py-5 ma-3 text-center"
          elevation="5"
        >
          <v-list-item three-line>
            <v-list-item-content>
              <v-list-item-title class="text-h7 mt-2">
                Osmose
              </v-list-item-title>
              <v-list-item-subtitle>
                          
                <v-chip v-if="values.s5 == false" label class="mt-2" color="green" text-color="white">Gut</v-chip>
                <v-chip v-else-if="values.s5 == true && values.s4 == false" label class="mt-2" color="orange" text-color="white">Ok</v-chip>
                <v-chip v-else-if="values.s5 == true && values.s4 == true" label class="mt-2" color="red" text-color="white">Alarm</v-chip>

              </v-list-item-subtitle>
            </v-list-item-content>

          </v-list-item>

          <v-card-actions>
          </v-card-actions>
        </v-card>        
      </v-flex>
      <v-flex xs6 sm3>
        <v-card
          class="py-5 ma-3 text-center"
          elevation="5"
        >
          <v-list-item three-line>
            <v-list-item-content>
              <v-list-item-title class="text-h7 mt-2">
                Temperatur
              </v-list-item-title>
              <v-list-item-subtitle>
                          
                <v-chip v-if="values.t1 >= 26" label class="mt-2" color="red" text-color="white">{{ values.t1 }} °C</v-chip>
                <v-chip v-else-if="values.t1 <= 23" label class="mt-2" color="blue" text-color="white">{{ values.t1 }} °C</v-chip>
                <v-chip v-else label class="mt-2" color="green" text-color="white">{{ values.t1 }} °C</v-chip>

              </v-list-item-subtitle>
            </v-list-item-content>

          </v-list-item>

          <v-card-actions>
          </v-card-actions>
        </v-card>        
      </v-flex>



      <v-flex xs12 sm12 class="pa-4"> <v-divider></v-divider> </v-flex>



      <v-flex xs12 sm6 class="pa-3">
        <v-card
          class="mx-auto text-center"
          dark
          max-width="600"
          height="100%"
        >
          <v-card-text>

            <div class="text-h5 pb-4">Pumpe Osmose</div>

            <v-sheet color="rgba(0, 0, 0, .12)" class="pt-5">

              

              <v-sparkline
                :value="stats_p1"
                color="rgba(255, 255, 255, .7)"
                height="100"
                smooth=10
                type="bar"
                line-width=20
                label-size="12"
                
              >
                <template v-slot:label="stats_p1" class="">
                  {{ (toLiter(stats_p1.value)/1000).toFixed(1) }}L
                </template>
              </v-sparkline>              

            </v-sheet>

          </v-card-text>

          <v-card-text>


            <v-chip v-if="values.p1 == true" label class="mb-5" color="orange" text-color="white">An</v-chip>
            <v-chip v-else label class="mb-5" text-color="white">Aus</v-chip>

            <v-progress-linear class="my-4" aria-valuemax=1000 :value=getRunPercent()></v-progress-linear>
            <h2 class="my-2">{{ values.p1_time }}s / {{ toLiter(values.p1_time) }}ml</h2>
            <v-divider class="mx-10"></v-divider>
            <h5 class="my-2">Zeit seit letztem manuellem reset:<br>{{ values.p1_time_reset }}s / {{ toLiter(values.p1_time_reset) }}ml</h5>               

          </v-card-text>

  
        </v-card> 
      </v-flex>

      <v-flex xs12 sm6 class="pa-3">
        <v-card
          class="mx-auto text-center"
          dark
          max-width="600"
          height="100%"
        >
          <v-card-text>

            <div class="text-h5 pb-4">Motor Filter</div>

            <v-sheet color="rgba(0, 0, 0, .12)" class="pt-5">
              <v-sparkline
                :value="stats_p2"
                color="rgba(255, 255, 255, .7)"
                height="100"
                smooth=10
                type="bar"
                line-width=20
                label-size="12"
                
              >
                <template v-slot:label="stats_p2" class="">
                  {{ stats_p2.value }}s
                </template>
              </v-sparkline>   
            </v-sheet>


          </v-card-text>


          <v-card-text>

            <v-chip v-if="values.p2 == true" label class="mb-5" color="orange" text-color="white">An</v-chip>
            <v-chip v-else label class="mb-5" text-color="white">Aus</v-chip>

            <h2 class="my-2">{{ values.p2_time }}s</h2>

          </v-card-text>

        </v-card> 
      </v-flex>



     <v-flex xs12 sm12 class="pa-4"> <v-divider></v-divider> </v-flex>



      <v-flex xs6 sm3 class="pa-1">
        <v-card
          class="py-5 ma-3 text-center"
          elevation="5"
        >
          <v-list-item three-line>
            <v-list-item-content>
              <v-list-item-title class="text-h7 mt-2 text-decoration-underline">
                Kalk
              </v-list-item-title>
              <v-list-item-subtitle>
                          
                <v-chip v-if="values.s7 == false" label class="mt-2" color="green" text-color="white">Gut</v-chip>
                <v-chip v-if="values.s7 == true" label class="mt-2" color="orange" text-color="white">Niedrig</v-chip>

              </v-list-item-subtitle>
            </v-list-item-content>

          </v-list-item>

          <v-card-actions>
          </v-card-actions>
        </v-card>        
      </v-flex>
      <v-flex xs6 sm3 class="pa-1">
        <v-card
          class="py-5 ma-3 text-center"
          elevation="5"
        >
          <v-list-item three-line>
            <v-list-item-content>
              <v-list-item-title class="text-h7 mt-2 text-decoration-underline">
                Calcium
              </v-list-item-title>
              <v-list-item-subtitle>
                          
                <v-chip v-if="values.s8 == false" label class="mt-2" color="green" text-color="white">Gut</v-chip>
                <v-chip v-if="values.s8 == true" label class="mt-2" color="orange" text-color="white">Niedrig</v-chip>

              </v-list-item-subtitle>
            </v-list-item-content>

          </v-list-item>

          <v-card-actions>
          </v-card-actions>
        </v-card>        
      </v-flex>
      <v-flex xs6 sm3 class="pa-1">
        <v-card
          class="py-5 ma-3 text-center"
          elevation="5"
        >
          <v-list-item three-line>
            <v-list-item-content>
              <v-list-item-title class="text-h7 mt-2 text-decoration-underline">
                Magnesium
              </v-list-item-title>
              <v-list-item-subtitle>
                          
                <v-chip v-if="values.s9 == false" label class="mt-2" color="green" text-color="white">Gut</v-chip>
                <v-chip v-if="values.s9 == true" label class="mt-2" color="orange" text-color="white">Niedrig</v-chip>

              </v-list-item-subtitle>
            </v-list-item-content>

          </v-list-item>

          <v-card-actions>
          </v-card-actions>
        </v-card>        
      </v-flex>
      <v-flex xs6 sm3 class="pa-1">
        <v-card
          class="py-5 ma-3 text-center"
          elevation="5"
        >
          <v-list-item three-line>
            <v-list-item-content>
              <v-list-item-title class="text-h7 mt-2 text-decoration-underline">
                Bakterien
              </v-list-item-title>
              <v-list-item-subtitle>
                          
                <v-chip v-if="values.s10 == false" label class="mt-2" color="green" text-color="white">Gut</v-chip>
                <v-chip v-if="values.s10 == true" label class="mt-2" color="orange" text-color="white">Niedrig</v-chip>

              </v-list-item-subtitle>
            </v-list-item-content>

          </v-list-item>

          <v-card-actions>
          </v-card-actions>
        </v-card>        
      </v-flex>


    </v-layout>

  </v-container>
</template>





<script>

  const prot = "https://";
  const ip = "api.avastack.de";
  const api = "/v2/aqui/"
  let path = prot + ip + api;

  let t;

  export default {
    data: () => ({ 
      values: [],
      settings: [],
      system: [],
      stats_p1: [0,0,0,0,0,0,0],
      stats_p2: [0,0,0,0,0,0,0],
      poller: null,
    }),
    methods: {
      toLiter(val) {
        return Math.floor(val*11.6666666666667);
      },
      getDataValues() {
        fetch(path+"values")
          .then(response => response.json())
          .then(data => (this.values = data));
      },
      getDataSettings() {
        fetch(path+"settings")
          .then(response => response.json())
          .then(data => (this.settings = data));
      },      
      getDataSystem() {
        fetch(path+"system")
          .then(response => response.json())
          .then(data => (this.system = data));
      }, 
      getDataStatsP1() {
        fetch(path+"stats/p1")
          .then(response => response.json())
          .then(data => (this.stats_p1 = data.reverse()));
      },
      getDataStatsP2() {
        fetch(path+"stats/p2")
          .then(response => response.json())
          .then(data => (this.stats_p2 = data.reverse()));
      },
      setReset(){
        fetch(path+"reset")
      },
      setResetAll(){
        fetch(path+"resetall")
      },
      setMaxpumptime(val){
        fetch(path+"write/maxpumptime?val="+val)
      },
      setTimeshift(val){
        fetch(path+"write/timeshift?val="+val)
      },
      getRunPercent(){
        return((this.values.p1_time/this.settings.p1_max_time)*100);
      }                        
    },
    mounted: function(){

      this.getDataValues();
      this.getDataSettings();
      this.getDataSystem();

      this.getDataStatsP1();
      this.getDataStatsP2();

      this.$root.$on.poller = setInterval(() => {
        this.getDataValues();
      }, 2000)      
    }

  }
</script>


