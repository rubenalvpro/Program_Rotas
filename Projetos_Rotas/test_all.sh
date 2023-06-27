#!/bin/bash
# File:  test_all.sh
# Author:  Vitor Custódio
# Created on:  21fev2020

DIFF="diff --ignore-space-change --ignore-blank-lines"

make clean
make
prog_make=$?
if [ ${prog_make} != 0 ]; then
    echo "ERRO: Erro de compilação"
    exit 1
else
    echo "OK:Programa corretamente compilado...."
fi

exefile="SGO_API"
if [ ! -f "${exefile}" ]; then
    echo "ERRO: Ficheiro executável ${exefile} não encontrado!"
    exit 1
else
    echo "**********************************"
    echo "******* TESTES AUTOMÁTICOS *******"
    echo "**********************************"
fi

for test_in in testes/*_*.in ; do
    echo "A Testar:" "${test_in}"
    test_out="${test_in%.in}.out"
    stamp="${RANDOM}${RANDOM}"
    student_out=/tmp/in_${stamp}
    #Switch
    case  ${test_in} in
        testes/test07_2.in) 
            if [ ! -f "data.sgo" ]; then
            echo "ERRO: Ficheiro data.sgo não foi criado em test07_1"
                continue
            fi
            ./${exefile} data.sgo <${test_in} | sed -e 's/\r$//' >${student_out}
        ;;
        testes/test07_3.in)
            ./${exefile} xpto.lll <${test_in} | sed -e 's/\r$//' >${student_out}
        ;;
        testes/test07_4.in)
            ./${exefile} xpto.sgo <${test_in} | sed -e 's/\r$//' >${student_out}
        ;;
        testes/test07_5.in)
            if [ ! -f "xpto.sgo" ]; then
            echo "ERRO: Ficheiro xpto.sgo não foi criado em test07_4"
                continue
            fi
            ./${exefile} xpto.sgo <${test_in} | sed -e 's/\r$//' >${student_out}
        ;;
        *)
            ./${exefile} <${test_in} | sed -e 's/\r$//' >${student_out}
        ;;
    esac
    rv_student=$?
    if [ ! -f "${student_out}" ]; then
        echo "ERRO: O output do Exercício não foi criado (file ${student_out})!"
        exit 1
    fi
    if [ ${rv_student} != 0 ]; then
        echo "ERRO: Programa executado com erros!"
        echo "REVEJA O CÓDIGO"
        rm -f ${student_out}
        exit 1
    else
        echo "OK: Programa executado sem erros..."
    fi
    
    ${DIFF} ${student_out} ${test_out}
    rv_diff=$?
    rm -f ${student_out}

    if [ ${rv_diff} == 0 ]; then
        echo "Teste ${test_in}:-> SUCESSO!"
    else
        echo "Teste ${test_in}:-> NÃO PASSOU. REVEJA O CÓDIGO!"
        echo " A efetuar make clean antes de sair"
        make clean
        exit 1
    fi
done
echo "**********************************"
echo "******* SUCESSO - PARABÈNS *******"
echo "**********************************"
echo "Lembre-se que estes testes não cobrem todos os casos"
echo "Deverá testar exaustivamente com outros testes!!!!"
echo " A efetuar make clean"
make clean
