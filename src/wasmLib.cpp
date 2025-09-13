#pragma once
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <sstream>
#include "plotter.h"
#include "Parser.h"
#include "proposicional.h"


inline std::string repl(std::string usrline) {
     static std::string mode = "math";
     static cartesianR2::vec2d domain = { -35.f, 35.f };
     static cartesianR2::vec2d domain2 = { -10.f, 10.f };

     std::ostringstream out;
     std::string line = usrline;

     if (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
         line.pop_back();

     if (line.empty()) return "";
     if (line == "exit" || line == "sair") return "__EXIT__";

     if (line == "math" || line == "logic") {
         mode = line;
         return "Modo alterado para: " + mode;
     }

     if (line == "help") {
         out << "\nComandos disponivel:\n";
         out << "  math                            - Muda para modo matemático\n";
         out << "  logic                           - Muda para modo lógico\n";
         out << "  const <k> = <valor>             - Define constante matemática (ex: const k = 2.5)\n";
         out << "  domain = <min> <max>            - Define domínio para análise (ex: domain = -10 10)\n";
         out << "  deriv <expr>                    - Deriva uma expressão simbolicamente (ex: deriv x^2)\n";
         out << "  deriv <expr> at <v>             - Calcula valor da derivada em x = v (ex: deriv sin(x) at 0)\n";
         out << "  integ <expr>                    - Integra uma expressão simbolicamente (ex: integ x^2)\n";
         out << "  integ <expr> from <v> to <v2>   - Calcula valor da integral em a,b = v v1 (ex: integ sin(x) at 0)\n";

         out << "  sair / exit / x       - Sai do programa\n\n";

         out << "Constantes matemáticas pré-definidas:\n";
         out << "  pi     ≈ 3.14159\n";
         out << "  e      ≈ 2.71828\n";
         out << "  tau    = 2*pi\n";
         out << "  phi    ≈ 1.61803\n";
         out << "  c      = 299792458 (velocidade da luz)\n";
         out << "  g      = 9.80665 (gravidade)\n";
         out << "  h      = 6.62607015e-34 (Planck)\n";
         out << "  k      = 5 (padrão, pode ser sobrescrito)\n\n";

         out << "Funções matemáticas disponivel:\n";
         out << "  Trigonométricas:      sin, cos, tan, asin, acos, atan\n";
         out << "  Hiperbólicas:         sinh, cosh, tanh\n";
         out << "  Logarítmicas:         log (base 10), ln (natural)\n";
         out << "  Exponenciais:         exp\n";
         out << "  Outros utilitários:   abs, sqrt, cbrt, ceil, floor, round, trunc\n";
         out << "  Sinais e misc:        sign, min, max, gcd, lcm\n";
         out << "  Valor absoluto:       |expr| (ex: |x+1|)\n";

         out << "Funções lógicas disponivel:\n";
         out << "  negação,conjunção,disjunção,implicação,equivalencia, e parenteses:      !, &&, ||,|,->, <->, ( p -> q) \n";



         out << "\nVariável padrão: x\n";
         out << "Exemplo de uso: sin(x)^2 + cos(x)^2\n";

         return out.str();
     }

     if (line.rfind("const ", 0) == 0) {
         std::istringstream iss(line.substr(6));
         std::string name, eq; double value;
         iss >> name >> eq >> value;
         if (eq == "=" && !name.empty() && iss) {
             parser::addConstant(name, value);
             return "Constante '" + name + "' = " + std::to_string(value);
         }
         return "Sintaxe incorreta. Use: const nome = valor";
     }

     if (line.rfind("domain", 0) == 0) {
         std::istringstream iss(line);
         std::string cmd, eq; float min, max;
         iss >> cmd >> eq >> min >> max;
         if (cmd == "domain" && eq == "=" && iss) {
             domain = { min, max };
             return "Domínio = [" + std::to_string(min) + "," + std::to_string(max) + "]";
         }
         return "Sintaxe incorreta. Use: domain = min max";
     }

     try {
         if (mode == "logic") {
             logica::Parser p(line);
             p.parse();
             std::ostringstream buf; p.eval(buf);
             return buf.str();
         }

         if (line.rfind("deriv ", 0) == 0) {
             std::string exprStr = line.substr(6);
             exprStr.erase(std::remove(exprStr.begin(), exprStr.end(), ' '), exprStr.end());
             double delta = 0.0; bool usedelta = false;
             auto atPos = exprStr.find("at");
             if (atPos != std::string::npos) {
                 delta = std::stod(exprStr.substr(atPos + 2));
                 exprStr = exprStr.substr(0, atPos);
                 usedelta = true;
             }
             parser::Parser parser(exprStr); parser.Parse();
             if (usedelta) {
                 auto f = parser.toFunction();
                 return std::to_string(cartesianR2::utils::derivada(f, delta));
             }
             return parser.ast.back()->deriv();
         }

         if (line.rfind("integ", 0) == 0) {
             std::string exprStr = line.substr(6);
             exprStr.erase(std::remove(exprStr.begin(), exprStr.end(), ' '), exprStr.end());
             auto fromPos = exprStr.find("from");
             auto toPos = exprStr.find("to");
             if (fromPos == std::string::npos || toPos == std::string::npos || toPos < fromPos)
                 return "Formato invalido. Use: integ <expr> from a to b";
             std::string expr = exprStr.substr(0, fromPos);
             double a = std::stod(exprStr.substr(fromPos + 4, toPos - (fromPos + 4)));
             double b = std::stod(exprStr.substr(toPos + 2));
             parser::Parser parser(expr); parser.Parse();
             auto f = parser.toFunction();
             return std::to_string(cartesianR2::utils::integralriemannstyle(f, a, b));
         }

         if (line.find('=') != std::string::npos) {
             auto eq_pos = line.find('=');
             line = line.substr(eq_pos + 1);
         }
         std::cerr << "DEBUG expr=[" << line << "] (size=" << line.size() << ")\n";
         parser::Parser p(line);
         p.Parse();

         if (p.isFunctionExpr()) {
             auto function = p.toFunction();

             std::ostringstream js;
             js << "{ \"plot\": " << cartesianR2::plotter_js(function, domain, 400) << ",";

             std::string simp;
             try {
                 parser::Parser derivParser(p.ast.back()->deriv());
                 derivParser.Parse();
                 simp = derivParser.ast.back()->simplify()->repr();
             }
             catch (...) {
                 simp = p.ast.back()->deriv();
             }

             js << "\"analysis\": {"
                 << "\"expr\": \"" << line << "\","
                 << "\"domain\": [" << domain.x << "," << domain.y << "],"
                 << "\"injective\": " << (cartesianR2::utils::B_injectora(function, domain) ? "true" : "false") << ","
                 << "\"subjective\": " << (cartesianR2::utils::B_sobrejectora(function, domain) ? "true" : "false") << ","
                 << "\"bijectora\": " << (cartesianR2::utils::B_Bijectora(function, domain) ? "true" : "false") << ","
                 << "\"impar\": " << (cartesianR2::utils::B_impar(function, domain) ? "true" : "false") << ","
                 << "\"par\": " << (cartesianR2::utils::B_par(function, domain) ? "true" : "false") << ","
                 << "\"periodica\": " << (cartesianR2::utils::B_periodica(function, domain) ? "true" : "false") << ","
                 << "\"derivative_at_0\": " << cartesianR2::utils::derivada(function) << ","
                 << "\"derivative_symbolic\": \"" << simp << "\","
                 << "\"integral\": " << cartesianR2::utils::integralriemannstyle(function, domain.x, domain.y)
                 << "}}";
             return js.str();
         }
         else {
             return std::to_string(p.eval());
         }
     }
     catch (const std::exception& e) {
         return std::string("Erro: ") + e.what();
     }
     catch (...) {
         return "Expressao invalida.";
     }
 }

namespace wasm_exports {

    double derivada(std::function<double(double)> f, double a = 0) {
        return cartesianR2::utils::derivada(f, a);
    }

    double integral(std::function<double(double)> f, double a, double b) {
        return cartesianR2::utils::integralriemannstyle(f, a, b);
    }

    bool injetora(std::function<double(double)> f, cartesianR2::vec2d d) {
        return cartesianR2::utils::B_injectora(f, d);
    }

    bool par(std::function<double(double)> f, cartesianR2::vec2d d) {
        return cartesianR2::utils::B_par(f, d);
    }

    bool impar(std::function<double(double)> f, cartesianR2::vec2d d) {
        return cartesianR2::utils::B_impar(f, d);
    }

    bool periodica(std::function<double(double)> f, cartesianR2::vec2d d) {
        return cartesianR2::utils::B_periodica(f, d);
    }

    std::string deriv_symbolic(const std::string& expr) {
        parser::Parser p(expr);
        p.Parse();
        return p.ast.back()->deriv();
    }

    double eval_expr(const std::string& expr) {
        parser::Parser p(expr);
        p.Parse();
        return p.eval();
    }

    std::string truth_table(const std::string& expr) {
        logica::Parser p(expr);
        p.parse();
        std::ostringstream buf;
        p.eval(buf);
        return buf.str();
    }
}

// ---- Bindings ----
EMSCRIPTEN_BINDINGS(Descartes) {
    emscripten::function("repl", &repl);

    emscripten::value_object<cartesianR2::vec2d>("vec2d")
        .field("x", &cartesianR2::vec2d::x)
        .field("y", &cartesianR2::vec2d::y);

    emscripten::function("derivada", &wasm_exports::derivada);
        emscripten::function("integral", &wasm_exports::integral);
        emscripten::function("injetora", &wasm_exports::injetora);
        emscripten::function("par", &wasm_exports::par);
        emscripten::function("impar", &wasm_exports::impar);
        emscripten::function("periodica", &wasm_exports::periodica);

        emscripten::function("evalExpr", &wasm_exports::eval_expr);
        emscripten::function("derivSymbolic", &wasm_exports::deriv_symbolic);

        emscripten::function("truthTable", &wasm_exports::truth_table);
}
 
/*
em++ wasmLib.h -o wasmLib.js \ -std=c++20 \ -s MODULARIZE -s EXPORT_NAME=Descartes \ -s "EXPORTED_RUNTIME_METHODS=['ccall','cwrap']" \ --bind
*/