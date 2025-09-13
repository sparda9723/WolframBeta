# WolframBeta
## my math tool implementation based on: mathplotlib,wolfram,octave,geogebra

<img width="1024" height="1024" alt="62e424c4-51d6-4d3f-8e17-13bb5c98386a" src="https://github.com/user-attachments/assets/178c968a-5867-4118-b4fb-a39157db03cb" />
[![license](https://img.shields.io/badge/license-MIT-blue)](LICENSE)
[![made-with](https://img.shields.io/badge/made%20with-WebAssembly-blueviolet)](#)

Um **mini-CAS (Computer Algebra System)** estilo **GeoGebra / WolframAlpha**, mas feito em  
**JavaScript + WebAssembly (c++)**.  
Permite calcular, derivar, integrar, analisar funções e ainda desenhar gráficos em tempo real.  

---

## ✨ Funcionalidades

- 🧮 **Aritmética básica** → `3+3`, `32-3*e`, `5^4`
- 📈 **Plotagem automática** de funções → `sin(x)`, `x^pi`, `e^x`
- 📊 **Análise simbólica** → par, ímpar, periódica, injetora, sobrejetora
- 📚 **Derivadas simbólicas** → `deriv x^2`, `deriv e^x`
- 🔢 **Derivadas numéricas** → `deriv x^2 at 90`
- ∫ **Integrais definidas** → `integ x^2 from 0 to 75`
- ⚡ **Constantes pré-definidas**:  
  `pi`, `e`, `tau`, `phi`, `c`, `g`, `h`, `k`
- ⌛  **Em breve muito mais..!**  

---
## 🖼️ Exemplo de uso

```text
~[math]> 32-3
= 29

~[math]> deriv e^x
= ln(2.718282) * 2.718282^(x) * 1

~[math]> deriv e^x at 3
= 20.085537

~[math]> integ e^x from 0 to 3
= 19.085537

~[math]>  1/sin(x)
plota, e analisa...

```
---

