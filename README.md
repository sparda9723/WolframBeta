# WolframBeta
## my math tool implementation based on: mathplotlib,wolfram,octave,geogebra

<img width="400" height="400" alt="62e424c4-51d6-4d3f-8e17-13bb5c98386a" src="https://github.com/user-attachments/assets/a6968313-bb12-4f6f-adeb-78ef98998d30" /><br>
<img width="75" height="75" alt="62e424c4-51d6-4d3f-8e17-13bb5c98386a" src="https://img.shields.io/badge/license-MIT-blue" />
<img width="125" height="125" alt="62e424c4-51d6-4d3f-8e17-13bb5c98386a" src="https://img.shields.io/badge/made%20with-WebAssembly-blueviolet" />

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
## preview
<img width="300" height="300" alt="image" src="https://github.com/user-attachments/assets/8f3c6a52-5650-4411-91b8-4ca4f071fe91" /> <img width="300" height="300" alt="image" src="https://github.com/user-attachments/assets/0dedc293-3b1b-47c2-a6f2-3fe0f34cbc14" />




---

