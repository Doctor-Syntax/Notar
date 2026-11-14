# Notar Language (NTR) V4

**Notar Language (NTR)** is een eenvoudige, dynamische programmeertaal met ondersteuning voor:

- Dynamische variabelen
- Constante en typevaste variabelen:
    ```ntr
    var x: fix = 10; //constant
    var x: pure = 10; //type vast
    ```
- Functies
- Arrays
- Objects
- If-statements
- While-loops
- Referenties
- Basis typecontrole
- Automatische typeconversie
- een paar standaard functies
- draaiend via een terminal
- ingebouwde commandos

---
# Status

NTR is momenteel actief in development. De taal is functioneel, maar sommige features kunnen nog veranderen. De volgende update (v5) zal focussen op classes en een systeem voor de standaard bibliotheek. Ook komen er waarschijnlijk import modules die linken mogelijk maken. 

getest:

- Linux (g++)
- Windows: nog niet getest
---

# terminal

In deze nieuwe versie (v4) van Notar heb ik een terminal toegevoegd evenals beter foutmeldingen en debugging info. Je kunt alles runnen vanuit de terminal en ook debug flags toevoegen. 

---

# Tic tac toe

Ik heb een voorbeeld programma er bij gezet die je kunt uitproberen als test. Als je het main programma start dan opent de terminal, vanuit daar kun je een programma draaien.

---

# Voorbeeld

```ntr
func greet(name){
    __out("Hello ", name, "\n");
}

var player = {
    name: "Alice",
    score: 10
};

greet(player.name);
```
---

# Build

## Linux

```bash
git clone https://github.com/Doctor-Syntax/Notar.git
cd Notar
make
```
om vervolgens het programma te draaien doe je: 
```bash
./main
```

## Windows

De makefile is origineel op linux gemaakt, dus om het te compileren op window raad ik aan naar de makefile te kijken en eventueel aan te passen waar nodig.

---

# Syntax

Voor de syntax en builtin funties neem een kijkje op SYNTAX.md 

---