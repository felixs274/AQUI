<template>
  <v-app id="inspire">



<v-app-bar
  app
  bottom
  prominent
  dense
  elevation="4"
>

<v-layout row wrap desktop-classic class="text-center pt-6">

  <v-flex xs4 sm4 class="">
    <v-btn icon to="/">
      <v-icon>mdi-view-dashboard</v-icon>
    </v-btn>
  </v-flex>

  <v-flex xs4 sm4 class="">
    <v-btn icon to="/settings">
      <v-icon>mdi-cog</v-icon>
    </v-btn>
  </v-flex>

  <v-flex xs4 sm4 class="">
    <v-btn icon to="/system">
      <v-icon>mdi-desktop-classic</v-icon>
    </v-btn>
  </v-flex>

</v-layout>
</v-app-bar>


    <v-main class="pt-10">
      <router-view></router-view>
    </v-main>
  </v-app>
</template>

<script>
  export default {
    data: () => ({

    }),
  }
</script>
