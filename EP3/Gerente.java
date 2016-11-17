/******************************************************************************
 *  Nome:   Bruno Rafael Arico                  8125459
 *          Nicolas Nogueira Lopes da Silva     9277541
 *
 *****************************************************************************/
import java.util.*;
import java.lang.*;

public class Gerente {

    static int total; //memoria real
    static int virtual; //memoria virtual
    int s; //tamanho da unidade de alocação
    int p; //tamanho da pagina
    int algespaco; 
    int algsubstitui;
    double dt;
    Queue<Processo> fila;
    BitSet Mtotal; //bitmap da memoria real
    BitSet Mvirtual; //bitmap da memoria virtual
    int realpages; //numero de paginas que cabem na Real
    int virtualpages; //numero de paginas que cabem na Virtual
    static TreeMap<Integer, Integer> bindv;
    static TreeMap<Integer, Integer> bindr;

    
    public class Processo {
        double t0;
        double tf;
        String nome;
        int b;
        int pid;
        int[] posacessos;
        double[] tempacessos;

        public Processo (String[] strargs, int pidt) {
            t0 = Double.parseDouble(strargs[0]);
            nome = strargs[1];
            tf = Double.parseDouble(strargs[2]);
            b = Integer.parseInt(strargs[3]);
            pid = pidt;
            int size = (strargs.length - 4)/2;
            posacessos = new int[size];
            tempacessos = new double[size];
            for (int i = 4, j = 0; i < strargs.length-1; i = i + 2, j++) {
                posacessos[j] = Integer.parseInt(strargs[i]);
                tempacessos[j] = Double.parseDouble(strargs[i+1]);
            }
        }
    }   

    public void carregarArquivo (String arquivo) {
        System.out.println("Carregando arquivo: " + arquivo + "\n");
        String str;
        int pidt = 1;
        In in = new In(arquivo);
    
        str = in.readLine();
        String sargs[] = str.split(" ");
        total = Integer.parseInt(sargs[0]);
        virtual = Integer.parseInt(sargs[1]);
    
        Mtotal = new BitSet (total);
        Mvirtual = new BitSet (virtual);
            
        s = Integer.parseInt(sargs[2]);
        p = Integer.parseInt(sargs[3]);

        virtualpages =(int) (((double)virtual/(double)s)/(double)p);
        realpages = (int)(((double)total/(double)s)/(double)p);

        System.out.println(virtualpages + " " + realpages);
        bindv = new TreeMap<Integer, Integer>();
        bindr = new TreeMap<Integer, Integer>();

        fila = new LinkedList<Processo>();

        while (!in.isEmpty()) {
            str = in.readLine();
            sargs = str.split(" ");
            fila.add(new Processo(sargs, pidt));
            pidt++;
        }
    }

    public int firstFit (Processo Proc) {
        int bits = Proc.b;
        int size = 0;
        int count = 1, beg = 0;
        while (size <= bits) size += s;
        for (int i = 0; i < virtual; i++) {
            if (!Mvirtual.get(i)) {
            beg = i;
            while (!Mvirtual.get(i) && i < virtual) {i++; count++;}
            if(count > size) {
                Mvirtual.set(beg, beg+size);
                System.out.println(Mvirtual.toString() + " " + Mvirtual.length());
                return beg;
            }
            else count = 0;
            }
        }
        System.out.println("Não foi possivel alocar a memoria");
        System.exit(-1);
        return beg;
    }

    static int here = 0;
    
    public int nextFit (Processo Proc) {
        int bits = Proc.b;
        int size = 0;
        int count = 1, beg = 0, i, ii;
        boolean flag = false;
        while (size <= bits) size += s;
        for (i = here; i < here + virtual ; i++){
            ii = (i % virtual); //para fazer nosso apontador ver toda a memoria
            if (ii == 0) count = 1; //a memoria não é circular
            if (!Mvirtual.get(ii) && count < size) { //se o bit for zero
            if (!flag) {beg = ii; flag = true;} //marca o comeco
            count++; //conta a sequencia de zeros
            }
            else { //quando a sequencia acaba
            flag = false;
            if (count >= size) { //se for maior que size, aloca
                Mvirtual.set(beg, beg+size);
                here = beg+size; //marca onde parou de olhar
                System.out.println(Mvirtual.toString() + " " + Mvirtual.length());
                return beg; // encerra a busca
            }
            else count = 0; //senão zera o contador e continua buscando
            }
        }
        System.out.println("Não foi possivel alocar a memoria");
        System.exit(-1);
        return beg;
    }

    public int bestFit (Processo Proc) {
        int bits = Proc.b;
        int size = 0;
        int provbeg = 0, realbeg = -1, count = 0, diff = Integer.MAX_VALUE;
        while (size <= bits) size += s;
        for (int i = 0; i < virtual; i++) {
            if (!Mvirtual.get(i)) {
                provbeg = i;
                while (!Mvirtual.get(i) && i < virtual) {i++; count++;}

                if (count > size && count-size < diff) {
                    realbeg = provbeg;
                    diff = count-size;
                }
                else count = 0;
            }
        }

        if (realbeg < 0) {
            System.out.println("Não foi possivel alocar a memoria");
            System.exit(-1);
        }
        else {
            Mvirtual.set(realbeg, realbeg+size);
            System.out.println(Mvirtual.toString() + " " + Mvirtual.length());
            return realbeg;
        }
        return realbeg;
    }

    public int worstFit (Processo Proc) {
        int bits = Proc.b;
        int size = 0;
        int provbeg = 0, realbeg = -1, count = 0, diff = -1;
        while (size <= bits) size += s;
        for (int i = 0; i < virtual; i++) {
            if (!Mvirtual.get(i)) {
            provbeg = i;
            while (!Mvirtual.get(i) && i < virtual) {i++; count++;}

            if (count > size && count-size > diff) {
                realbeg = provbeg;
                diff = count-size;
            }
            else count = 0;
            }
        }

        if (realbeg < 0) {
            System.out.println("Não foi possivel alocar a memoria");
            System.exit(-1);
        }
        else {
            Mvirtual.set(realbeg, realbeg+size);
            System.out.println(Mvirtual.toString() + " " + Mvirtual.length());
            return realbeg;
        }
        return realbeg;
    }

    public void binding (int Virtual, int Real) {
        bindv.put(Virtual, Real);
        bindr.put(Real, Virtual);
    }

    public int MMU (int Virt) {
        int Vblock,Rblock, sector,fin;
        Vblock = (int)(Virt/p); //encontro sobre qual pagina age a instrução
        Rblock = bindv.get(Vblock); //acho a equivalente na memoria Real
        sector = Virt % s; //acho o setor da pagina
        fin = Rblock*s + sector; //Faz a translação do bloco
        System.out.println(Rblock + " " + sector + " " + fin);
        return fin;
    }

    //clase interna criada para ordenar vetor de counters e manter índices (páginas)

    public class Pair implements Comparable<Pair> {
        public final int index;
        public final int value;

        public Pair(int index, int value) {
            this.index = index;
            this.value = value;
        }

        @Override
        public int compareTo(Pair other) {
            //multiplied to -1 as the author need descending sort order
            return -1 * Integer.valueOf(this.value).compareTo(other.value);
        }
    }

    static int[] countoptimal;
    static ArrayList<Pair> counters;

    public void preProcessOptimal () {
        countoptimal = new int[virtualpages];
        counters = new ArrayList<Pair>();
        for (Processo Proc : this.fila)
            for (int i : Proc.posacessos)
                countoptimal[(int)(i/p)]++; //CONTA PRA ACHAR A PÁGINA (ACHO QUE ESTÁ ERRADA)
        System.out.println("DESORDENADO");
        for (int i = 0; i < virtualpages; i++)
            System.out.print(i + " - " + countoptimal[i] + "; ");
        for (int i = 0; i < virtualpages; i++)
            counters.add(new Pair(i, countoptimal[i]));
        Collections.sort(counters); //na teoria ta ordenado pelo valor
        System.out.println("ORDENADO");
        for (Pair pair : counters)
            System.out.print(pair.index + " - " + pair.value + "; ");
    }
    
    public int optimal (int pos, int pid) {
        if (bindv.get(pos) > 0) {
            System.out.println("Optimal: posicao esta na mem fisica!");
        } else {
            System.out.println("Optimal: realizando operacao de substituicao.");
            if (!counters.isEmpty()) {
                Pair temp = counters.remove(0);
                int t = bindr.get(temp.index);
                bindv.put(pos, temp.index);
                bindr.put(t, -1);
                //executa operação do optimal
                //substituir o conteúdo no arquivo                
            }

        }
        return 1;
    }

    // test client
    public static void main(String[] args) {
        Gerente gerente = new Gerente();
        while (true) {
            System.out.print("(ep3): ");
            String comando = StdIn.readString();
            if (comando.equals("carrega")) {
                gerente.carregarArquivo (StdIn.readString());
                //precisa preprocessar no optimal
                gerente.preProcessOptimal();
                System.out.println("Sonic");
                System.out.println("A posição é:" + gerente.worstFit(gerente.fila.remove()));
                System.out.println("tails");
                System.out.println("A posição é:" + gerente.worstFit(gerente.fila.remove()));
                gerente.binding(2, 5);
                gerente.MMU(20);
            } else if (comando.equals("espaco")) {
                gerente.algespaco = StdIn.readInt();
                System.out.println(gerente.algespaco);
            } else if (comando.equals("substitui")) {
                gerente.algsubstitui = StdIn.readInt();
                System.out.println(gerente.algsubstitui);
            } else if (comando.equals("executa")) {
                gerente.dt = StdIn.readDouble();
                System.out.println(gerente.dt);
            } else if (comando.equals("sai")) {
                System.out.println("saiu!");
                break;
            } else {
                System.out.println("Comando desconhecido\n");
            }
        }
    }
}