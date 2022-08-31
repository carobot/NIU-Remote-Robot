// Import the functions you need from the SDKs you need
//import { initializeApp } from 'https://www.gstatic.com/firebasejs/9.9.1/firebase-app.js';
import { getAnalytics } from 'https://www.gstatic.com/firebasejs/9.9.1/firebase-analytics.js';
import { getAuth, onAuthStateChanged } from 'https://www.gstatic.com/firebasejs/9.9.1/firebase-auth.js';
import {getFirestore} from 'https://www.gstatic.com/firebasejs/9.9.1/firebase-firestore.js'
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.9.1/firebase-app.js";
import { getDatabase, ref, set } from "https://www.gstatic.com/firebasejs/9.9.1/firebase-database.js";


// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "[insertapikey]",
  authDomain: "quick-woodland-357917.firebaseapp.com",
  projectId: "quick-woodland-357917",
  storageBucket: "quick-woodland-357917.appspot.com",
  messagingSenderId: "369063394646",
  appId: "1:369063394646:web:f202cf66a227f031e7aaf7",
  measurementId: "G-WN1JEQRDXQ",
  databaseURL: "https://quick-woodland-357917-default-rtdb.firebaseio.com"

};
function writeUserData(leftnum, rightnum, shootnum, camnum) {
    const db = getDatabase();
    set(ref(db, 'data'), {
      cam: camnum,
      left: leftnum,
      right: rightnum,
      shoot: shootnum
    });
  }


  
  // Initialize Firebase
  const app = initializeApp(firebaseConfig);
  
  
  // Initialize Realtime Database and get a reference to the service
  const database = getDatabase(app);
  

// Initialize Firebase
const analytics = getAnalytics(app);

const auth = getAuth(app);
const db = getFirestore(app);

// Detect auth state

onAuthStateChanged(auth, user => {
    if(user != null){
        console.log("logged in!");
    } else {
        console.log("No user");
    }
});

var R = 0;
var L = 0;
var S = 0;
var cam =0;

window.addEventListener('keydown', e =>{
    console.log(e)
    // camera 'c'
    if (e.keyCode === 67) {
        document.querySelector(".arr.camera").style.backgroundColor ='blue';
        cam++;
        if(cam == 5){
            cam =0;
        }
        writeUserData(L, R, S, cam);
        }
    // right key
    if (e.keyCode === 39) {
        document.querySelector(".arr.right").style.backgroundColor ='blue';
        R = -100;
        L = 100;
        writeUserData(L, R, S, cam);
        }
    // left key
    else if (e.keyCode === 37) {
        document.querySelector(".arr.left").style.backgroundColor ='blue';
        R = 100;
        L = -100;
        writeUserData(L, R, S, cam);
    }
    // up key
    else if (e.keyCode === 38) {
        document.querySelector(".arr.up").style.backgroundColor ='blue';
        R = 100;
        L = 100;
        writeUserData(L, R, S, cam);
    }
    // down key
    else if (e.keyCode === 40) {
        document.querySelector(".arr.down").style.backgroundColor ='blue';
        R = -100;
        L = -100;
        writeUserData(L, R, S, cam);
    }
    // s key
    else if (e.keyCode === 83) {
        document.querySelector(".shoot").style.backgroundColor ='blue';
        S = 1;
        writeUserData(L, R, S, cam);
    }

})

window.addEventListener('keyup', e =>{
    console.log(e)
    // right key
    if (e.keyCode === 39) {
        document.querySelector(".arr.right").style.backgroundColor ='white';
        R = 0;
        L = 0;
        writeUserData(L, R, S, cam);
        }
    // left key
    else if (e.keyCode === 37) {
        document.querySelector(".arr.left").style.backgroundColor ='white';
        R = 0;
        L = 0;
        writeUserData(L, R, S, cam);
    }
    // up key
    else if (e.keyCode === 38) {
        document.querySelector(".arr.up").style.backgroundColor ='white';
        R = 0;
        L = 0;
        writeUserData(L, R, S, cam);
    }
    // down key
    else if (e.keyCode === 40) {
        document.querySelector(".arr.down").style.backgroundColor ='white';
        R = 0;
        L = 0;
        writeUserData(L, R, S, cam);
    }
    // s key
    else if (e.keyCode === 83) {
        document.querySelector(".shoot").style.backgroundColor ='white';
        S = 0;
        writeUserData(L, R, S, cam);
    }

})


