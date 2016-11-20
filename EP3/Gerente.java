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
    static Queue<Double>[] countoptimal;
    TreeMap<Integer, Integer> bindr;
    TreeMap<Integer, Integer> bindv;

	
    public class Processo {
        double t0;
        double tf;
        String nome;
        int b;
        int pid;
	int offset;
        int[] posacessos;
        double[] tempacessos;

	public void setOffset(int value) {
	    offset = value;
	}
	
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
	System.out.println("vp = " +virtualpages);
	countoptimal = new Queue[virtualpages];
	System.out.println(virtualpages + " " + realpages);
	
	for(int i = 0; i < virtualpages; i++) 
	    countoptimal[i] = new LinkedList<Double>(); 

	bindv = new TreeMap<Integer, Integer>();
        bindr = new TreeMap<Integer, Integer>();
	fila = new LinkedList<Processo>();
	
        while (!in.isEmpty()) {
            str = in.readLine();
	    System.out.println(str +" aqui");
            sargs = str.split(" ");
	    fila.add(new Processo(sargs, pidt));
            pidt++;
        }
    }

    public void firstFit (Processo Proc) {
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
		    Proc.setOffset(beg);
		}
		else count = 0;
	    }
	}
	System.out.println("Não foi possivel alocar a memoria");
	System.exit(-1);
        return;
    }

    static int here = 0;
    
    public void nextFit (Processo Proc) {
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
		     Proc.setOffset(beg); // encerra a busca
		}
		else count = 0; //senão zera o contador e continua buscando
	    }
	}
        System.out.println("Não foi possivel alocar a memoria");
	System.exit(-1);
	return;
    }

    public void bestFit (Processo Proc) {
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
	    Proc.setOffset(realbeg);
	}
	return;
    }

    public void worstFit (Processo Proc) {
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
	     Proc.setOffset(realbeg);
	}
	return;
    }

    //mapeia uma pagina da Virtual na memoria Real
    public void binding (int Virtual, int Real) {
        bindv.remove(bindr.get(Real)); // REMOVE V -> R
        bindv.put(Virtual, Real); // ADICIONA NV -> R
        bindr.put(Real, Virtual); // MODIFICA R -> V para R -> NV

    }

    //Devolve o equivalente de uma instrução na memoria virtual na real
    public int MMU (int Virt) {
	int Vblock,Rblock, sector,fin;
	Vblock = (Virt/p); //encontro sobre qual pagina age a instrução
	Rblock = bindv.get(Vblock);; //acho a equivalente na memoria Real
	sector = Virt % s; //acho o setor da pagina
	fin = Rblock * s + sector; //Faz a translação do bloco
	System.out.println(Rblock + " " + sector + " " + fin);
        return fin;
    }

    public  void preProcessOptimal (Processo Proc) {
	System.out.println(Proc.offset);
	for (int i = 0; i < Proc.posacessos.length; i++){
	    System.out.println((Proc.posacessos[i]+Proc.offset)/s/p);
	    countoptimal[(Proc.posacessos[i]+Proc.offset)/s/p].add(Proc.tempacessos[i]);
	}
    }

    //recebe uma posição na memoria virtual pos*/
    public void optimal (int pos, int pid) {
        int accblk = pos/s/p, index;
	double buffer, larger = 0;
	if(!bindv.containsKey(accblk)) {//verifica se tem o bloco associado a real
	    //se não tiver
	    
	    if(bindr.size() < realpages){ //ve se tem espaço livre na memoria real
	        for(int i = 0; i < realpages; i++) //se tiver procura
		    if(!bindr.containsKey(i)){ 
			Mtotal.set(i*s, (i+1)*s); //preenche esse bloco do bitset
			return;
		    }
	    }
	    
	    else {
		for(int i : bindr.values()) { //percorre todos que estão na real
		    if(countoptimal[i].isEmpty()) { //se a pilha de alguem estiver vazia
			binding(accblk, bindv.get(i)); //associo o novo bloco ao da antiga
			return;
		    }
		    else{ //senão procuro pelo que vai ficar mais tempo sem acesso
			buffer = countoptimal[i].peek(); //olha o topo da fila
			if (buffer >= larger) {large = buffer; index = i;} //se o acesso for mais demorado troca
		    }   
		}
		binding(accblk, bindv.get(index));
		return;
	    }	    
	}
    }
	
    // test client
    public static void main(String[] args) {
        Gerente gerente = new Gerente();
        while (true) {
            System.out.print("(ep3): ");
            String comando = StdIn.readString();
            if (comando.equals("carrega")) {
		Processo x;
                gerente.carregarArquivo (StdIn.readString());
		x = gerente.fila.remove();
		gerente.worstFit(x);
		System.out.println("POsiçoes acesadas");
		gerente.preProcessOptimal(x);
		x = gerente.fila.remove();
		gerente.worstFit(x);
		System.out.println("POsiçoes acesadas");
		gerente.preProcessOptimal(x);
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
