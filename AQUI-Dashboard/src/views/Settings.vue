<template v-slot:default>
<v-container class="my-4" >
  <v-simple-table dark>
      <thead>
        <tr>
          <th class="text-left">
            Einstellung
          </th>
          <th class="text-left">
            Wert
          </th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td>Sommerzeit</td>
          <td> 
            <v-switch v-if="settings.timeshift == 0" v-on:change="setTimeshift(1)"></v-switch> 
            <v-switch v-else-if="settings.timeshift == 3600" v-on:change="setTimeshift(0)" :input-value="true"></v-switch> 
          </td>
        </tr>
        <tr>
          <td>P1 maximale Laufzeit</td>
          <td> 
            <v-text-field v-model="seconds" :placeholder=String(settings.p1_max_time) suffix="Sekunden"></v-text-field>
            <p v-if="seconds">ca. {{ toLiter(seconds) }}ml</p>
            <p v-else>ca. {{ toLiter(settings.p1_max_time) }}ml</p>
            <v-chip class="mb-5" v-on:click="setMaxpumptime(seconds)" color="blue">Sichern</v-chip>
          </td>
        </tr>
      </tbody>
  </v-simple-table>


  <v-layout row wrap justify-space-around class="text-center">

    <v-flex xs12 sm12 class="pt-10 px-10"> <v-divider></v-divider> </v-flex>

    <v-flex xs12 sm12 class="pa-10">
      <v-dialog
        v-model="resetdialog"
        persistent
        max-width="290"
      >
        <template v-slot:activator="{ on, attrs }">
          <v-btn
            color="orange"
            dark
            v-bind="attrs"
            v-on="on"
            x-large
          >
            Reset
          </v-btn>
        </template>
        <v-card>
          <v-card-title class="text-h5">
            Bist du sicher?
          </v-card-title>
          <v-card-text>Dieser Knopf setzt die aktuelle Pumpen Laufzeit zurück!</v-card-text>
          <v-card-actions>
            <v-spacer></v-spacer>
            <v-btn
              color="green"
              text
              @click="resetdialog = false"
            >
              Abbrechen
            </v-btn>
            <v-btn
              color="red"
              text
              @click="resetdialog = false; setReset();"
            >
              Ja
            </v-btn>
          </v-card-actions>
        </v-card>
      </v-dialog>      
    </v-flex>

    <v-flex xs12 sm12 class="pa-10">
    <v-dialog
      v-model="resetalldialog"
      persistent
      max-width="290"
    >
      <template v-slot:activator="{ on, attrs }">
        <v-btn
          color="red"
          dark
          v-bind="attrs"
          v-on="on"
          x-large
        >
          Reset Alles
        </v-btn>
      </template>
      <v-card>
        <v-card-title class="text-h5">
          Bist du sicher?
        </v-card-title>
        <v-card-text>Dieser Knopf setzt alle Pumpen Laufzeiten zurück!</v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn
            color="green"
            text
            @click="resetalldialog = false"
          >
            Abbrechen
          </v-btn>
          <v-btn
            color="red"
            text
            @click="resetalldialog = false; setResetAll();"
          >
            Ja
          </v-btn>
        </v-card-actions>
      </v-card>
    </v-dialog>      
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
      resetdialog: null,
      resetalldialog: null,
      seconds: "",
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
      }         
    },
    mounted: function(){

      clearInterval(this.$root.$on.poller);
      this.getDataSettings();
      this.getDataSystem();

    
    }

  }
</script>