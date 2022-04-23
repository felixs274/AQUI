<template>
<v-container class="pb-12">

  <v-layout row wrap justify-space-around class="text-center">

    <v-flex xs12 sm4 class="pt-10 px-10"> 
      <v-card elevation="5">
        <v-card-title class="text-h5"><v-icon class="mr-4">mdi-thermometer</v-icon>Intern 1</v-card-title>
        <v-divider></v-divider>
        <h1 v-if="system.t2" class="pa-4 text-left">{{Math.round(system.t2)}} °C</h1>
        <v-progress-circular v-else indeterminate color="primary" :size="40" class="ma-5"></v-progress-circular>
      </v-card>
    </v-flex>

    <v-flex xs12 sm4 class="pt-10 px-10"> 
      <v-card elevation="5">
        <v-card-title class="text-h5"><v-icon class="mr-4">mdi-thermometer</v-icon>Intern 2</v-card-title>
        <v-divider></v-divider>
        <h1 v-if="system.t3" class="pa-4 text-left">{{Math.round(system.t3)}} °C</h1>
        <v-progress-circular v-else indeterminate color="primary" :size="40" class="ma-5"></v-progress-circular>
      </v-card>
    </v-flex>

    <v-flex xs12 sm4 class="pt-10 px-10"> 
      <v-card elevation="5">
        <v-card-title class="text-h5"><v-icon class="mr-4">mdi-console-network-outline</v-icon>I²C Geräte</v-card-title>
        <v-divider></v-divider>
        <h1 v-if="system.i2c >= 0" class="pa-4 text-left">{{system.i2c}}</h1>
        <v-progress-circular v-else indeterminate color="primary" :size="40" class="ma-5"></v-progress-circular>
      </v-card>
    </v-flex>

    <v-flex xs12 sm4 class="pt-10 px-10"> 
      <v-card elevation="5">
        <v-card-title class="text-h5"><v-icon class="mr-4">mdi-wifi</v-icon>WiFi</v-card-title>
        <v-divider></v-divider>
        <h1 v-if="system.rssi" class="pa-4 text-left">{{system.rssi}} dbm</h1>
        <v-progress-circular v-else indeterminate color="primary" :size="40" class="ma-5"></v-progress-circular>
      </v-card>
    </v-flex>

    <v-flex xs12 sm4 class="pt-10 px-10"> 
      <v-card elevation="5">
        <v-card-title class="text-h5"><v-icon class="mr-4">mdi-clock-outline</v-icon>Lokaler timestamp</v-card-title>
        <v-divider></v-divider>
        <h1 v-if="system.localepoch" class="pa-4 text-left">{{system.localepoch}}</h1>
        <v-progress-circular v-else indeterminate color="primary" :size="40" class="ma-5"></v-progress-circular>
      </v-card>
    </v-flex>

    <v-flex xs12 sm4 class="pt-10 px-10"> 
      <v-card elevation="5">
        <v-card-title class="text-h5"><v-icon class="mr-4">mdi-clock-outline</v-icon>Lokale Zeit</v-card-title>
        <v-divider></v-divider>
        <h1 v-if="system.localtime" class="pa-4 text-left">{{system.localtime}}</h1>
        <v-progress-circular v-else indeterminate color="primary" :size="40" class="ma-5"></v-progress-circular>
      </v-card>
    </v-flex>

    <v-flex xs12 sm4 class="pt-10 px-10"> 
      <v-card elevation="5">
        <v-card-title class="text-h5"><v-icon class="mr-4">mdi-clock-outline</v-icon>Uptime</v-card-title>
        <v-divider></v-divider>
        <h1 v-if="system.uptime" class="pa-4 text-left">{{ Math.floor((system.uptime)/60) }} min</h1>
        <v-progress-circular v-else indeterminate color="primary" :size="40" class="ma-5"></v-progress-circular>
      </v-card>
    </v-flex>

    <v-flex xs12 sm4 class="pt-10 px-10"> 
      <v-card elevation="5">
        <v-card-title class="text-h5"><v-icon class="mr-4">mdi-cpu-32-bit</v-icon>Hardware</v-card-title>
        <v-divider></v-divider>
        <h1 v-if="system.pcb" class="pa-4 text-left">{{system.pcb}}</h1>
        <v-progress-circular v-else indeterminate color="primary" :size="40" class="ma-5"></v-progress-circular>
      </v-card>
    </v-flex>

    <v-flex xs12 sm4 class="pt-10 px-10"> 
      <v-card elevation="5">
        <v-card-title class="text-h5"><v-icon class="mr-4">mdi-code-tags</v-icon>Firmware</v-card-title>
        <v-divider></v-divider>
        <h1 v-if="system.firmware" class="pa-4 text-left">{{system.firmware}}</h1>
        <v-progress-circular v-else indeterminate color="primary" :size="40" class="ma-5"></v-progress-circular>
      </v-card>
    </v-flex>

    <v-flex xs12 sm12 class="pt-12 px-12"> <v-divider></v-divider> </v-flex>

    <v-flex xs12 sm12 class="pt-12 px-12 pb-4 text-center text-caption">
      <p>Made by Felix Scholzen</p>
      <a href="https://github.com/B0lzn" target="_blank">GitHub</a>
    </v-flex>

  </v-layout>

</v-container>
</template>

    

<script>

  const prot = "https://";
  const ip = "api.avastack.de";
  const api = "/v2/aqui/"
  let path = prot + ip + api;

  export default {
    data: () => ({ 
      settings: [],
      system: [],
    }),
    methods: {
      toLiter(val) {
        return Math.floor(val*11.6666666666667);
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
      }       
    },
    mounted: function(){

      clearInterval(this.$root.$on.poller);
      this.getDataSettings();
      this.getDataSystem();

    
    }

  }
</script>