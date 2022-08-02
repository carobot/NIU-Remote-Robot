// Import the functions you need from the SDKs you need
import { initializeApp } from 'https://www.gstatic.com/firebasejs/9.9.1/firebase-app.js';
import { getAnalytics } from 'https://www.gstatic.com/firebasejs/9.9.1/firebase-analytics.js';
import { getAuth, onAuthStateChanged } from 'https://www.gstatic.com/firebasejs/9.9.1/firebase-auth.js';
import {getFirestore} from 'https://www.gstatic.com/firebasejs/9.9.1/firebase-firestore.js'

// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyBGxF7-eRsRwxLGNxNvqUWlpY3FYNweUsg",
  authDomain: "quick-woodland-357917.firebaseapp.com",
  projectId: "quick-woodland-357917",
  storageBucket: "quick-woodland-357917.appspot.com",
  messagingSenderId: "369063394646",
  appId: "1:369063394646:web:f202cf66a227f031e7aaf7",
  measurementId: "G-WN1JEQRDXQ"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
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


window.addEventListener('keydown', e =>{
    console.log(e)
    // right key
    if (e.keyCode === 39) {
        document.querySelector(".arr.right").style.backgroundColor ='blue';
        }
    // left key
    else if (e.keyCode === 37) {
        document.querySelector(".arr.left").style.backgroundColor ='blue';
    }
    // up key
    else if (e.keyCode === 38) {
        document.querySelector(".arr.up").style.backgroundColor ='blue';
    }
    // down key
    else if (e.keyCode === 40) {
        document.querySelector(".arr.down").style.backgroundColor ='blue';
    }
    // s key
    else if (e.keyCode === 83) {
        document.querySelector(".shoot").style.backgroundColor ='blue';
    }

})

window.addEventListener('keyup', e =>{
    console.log(e)
    // right key
    if (e.keyCode === 39) {
        document.querySelector(".arr.right").style.backgroundColor ='white';
        }
    // left key
    else if (e.keyCode === 37) {
        document.querySelector(".arr.left").style.backgroundColor ='white';
    }
    // up key
    else if (e.keyCode === 38) {
        document.querySelector(".arr.up").style.backgroundColor ='white';
    }
    // down key
    else if (e.keyCode === 40) {
        document.querySelector(".arr.down").style.backgroundColor ='white';
    }
    // s key
    else if (e.keyCode === 83) {
        document.querySelector(".shoot").style.backgroundColor ='white';
    }

})



