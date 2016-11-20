/******************************************************************************
 *  Nome:   Bruno Rafael Arico                  8125459
 *          Nicolas Nogueira Lopes da Silva     9277541
 *
 *****************************************************************************/
import java.util.*;
import java.lang.*;
import java.io.*;

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
    static ArrayList<Queue<Double>> countoptimal;
    TreeMap<Integer, Integer> bindr;
    TreeMap<Integer, Integer> bindv;
    boolean pagebit[];
    ArrayList<Cell> eventos;
	
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
	pagebit = new boolean[virtualpages];
	System.out.println("vp = " +virtualpages);
	countoptimal = new ArrayList<Queue<Double>>();
	System.out.println(virtualpages + " " + realpages);
	
	for(int i = 0; i < virtualpages; i++) 
	    countoptimal.add(i, new LinkedList<Double>()); 

	bindv = new TreeMap<Integer, Integer>();
        bindr = new TreeMap<Integer, Integer>();
	fila = new LinkedList<Processo>();
	
        while (!in.isEmpty()) {
            str = in.readLine();
	    System.out.println(str);
            sargs = str.split(" ");
	    fila.add(new Processo(sargs, pidt));
            pidt++;
        }
	 fprintInicial("/tmp/ep3.mem", total);
        fprintInicial("/tmp/ep3.vir", virtual);
    }

    public void firstFit (Processo Proc) {
	int bits = Proc.b;
	int size = 0;
	int count = 0, beg = 0;
	while (size <= bits) size += s;
	for (int i = 0; i < virtual; i++) {
	    if (!Mvirtual.get(i)) {
		beg = i;
		while (!Mvirtual.get(i) && i < virtual) {i++; count++;}
		if(count > size) {
		    Mvirtual.set(beg, beg+size);
		    //imprimindo no arquivo da memoria virtual
		    for (int j = beg; j < beg+size; j++) {
		    	fprintPagina("/tmp/ep3.vir", Proc.pid, j, s*p);
		    }
		    //System.out.println(Mvirtual.toString());
		    Proc.setOffset(beg);
		    return;
		}
		else count = 0;
	    }
	}
	System.out.println("Não foi possivel alocar a memoria de tam " + size);
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
		    //imprimindo no arquivo da memoria virtual
		    for (int j = beg; j < beg+size; j++) {
		    	fprintPagina("/tmp/ep3.vir", Proc.pid, j, s*p);
		    }
		    here = beg+size; //marca onde parou de olhar
		    System.out.println(Mvirtual.toString());
		     Proc.setOffset(beg); // encerra a busca
		     return;
		}
		else count = 0; //senão zera o contador e continua buscando
	    }
	}
        System.out.println("Não foi possivel alocar a memoria para " + size);
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
		//imprimindo no arquivo da memoria virtual
		for (int j = realbeg; j < realbeg+size; j++) {
		    fprintPagina("/tmp/ep3.vir", Proc.pid, j, s*p);
		}
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
		//imprimindo no arquivo da memoria virtual
		for (int j = realbeg; j < realbeg+size; j++) {
		    fprintPagina("/tmp/ep3.vir", Proc.pid, j, s*p);
		}
	    System.out.println(Mvirtual.toString() + " " + Mvirtual.length());
	     Proc.setOffset(realbeg);
	}
	return;
    }

    //mapeia uma pagina da Virtual na memoria Real
    public void binding (int Virtual, int Real) {
	if(bindr.containsKey(Real)) bindv.remove(bindr.get(Real)); // REMOVE V -> R
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
        countoptimal.get((Proc.posacessos[i]+Proc.offset)/s/p).add(Proc.tempacessos[i]);
	    //countoptimal[(Proc.posacessos[i]+Proc.offset)/s/p].add(Proc.tempacessos[i]);
	}
    }

    //recebe uma posição na memoria virtual pos*/
    public void optimal (int pos) {
        int accblk = (int) pos/s/p, index = 0;
	double buffer, larger = 0;
	System.out.println("unidade " + s + " paginas " + p);
	System.out.println("pos " + pos + " accblk " + accblk);
	if(!bindv.containsKey(accblk)) {//verifica se tem o bloco associado a real
	    //se não tiver
	    System.out.println(accblk);
	    if(bindr.size() < realpages){ //ve se tem espaço livre na memoria real
	        for(int i = 0; i < realpages; i++) //se tiver procura
		    if(!bindr.containsKey(i)){
			System.out.println("entrou aqui");
			Mtotal.set(i*s, (i+1)*s); //preenche esse bloco do bitset
			System.out.println(Mtotal.toString());
			binding(accblk, i);
			return;
		    }
	    }
	    
	    else {
		for(int i : bindr.values()) { //percorre todos que estão na real
		    if(countoptimal.get(i).isEmpty()) { //se a pilha de alguem estiver vazia
			binding(accblk, bindv.get(i)); //associo o novo bloco ao da antiga
			System.out.println(Mtotal.toString());
			System.out.println("Pilha do Bloco " + i +" vazia");
			return;
		    }
		    else { //senão procuro pelo que vai ficar mais tempo sem acesso
			buffer = countoptimal.get(i).peek(); //olha o topo da fila
			if (buffer >= larger) {larger = buffer; index = i;} //se o acesso for mais demorado troca
			System.out.println("Olhando a pilha");
		    }   
		}
		binding (accblk, bindv.get(index));
		return;
	    }	    
	}
	else System.out.println("Ela existe");
    }
    static Queue<Integer> SC = new LinkedList<Integer>();
    
    
    public void secondChance (int pos) {
	int accblk = (int)pos/s/p;
	int buffer, firstbuffer;
	System.out.println("pos " + pos + " accblk " + accblk);
	if(!bindv.containsKey(accblk)) {
	     if(bindr.size() < realpages){
		 for (int i = 0; i < realpages; i++) //se tiver procura
		     if(!bindr.containsKey(i)){ //encontrou 
			 Mtotal.set(i*s, (i+1)*s); //preenche
			 binding (accblk, i); //faz a associação
			 SC.add(accblk); //entra na fila
			 System.out.println("Tinha espaço");
			 System.out.println(Mtotal.toString());
			 pagebit[accblk] = true; //liga o bit de acesso
			 return;
		     }
	     }
	     
	     else {
		 buffer = firstbuffer = SC.remove(); //tira da fila
		 while (pagebit[buffer]){ //olha o bit
		     System.out.println("O bit de " + buffer + "é true");
		     SC.add(buffer); //se for true devolve
		     buffer = SC.remove(); //tenta o proximo
		     if(buffer == firstbuffer) break; //se voltou no primeiro usa o ele
		 }
		 System.out.println("O espaço foi alocado ");
		 System.out.println(Mtotal.toString());
		 binding (accblk, bindv.get(buffer));
		 return;
	     }
		 
	}
	else{
	    pagebit[accblk] = true; //so liga o bit de acesso
	    System.out.println("acessou a pagina " + accblk );
	    return;
	}
    }

    static LinkedList<Integer> CList = new LinkedList<Integer>();
    static int pointer = 0;
    
    public void clock(int pos) {
	int accblk = pos/s/p, buffer;
	if(!bindv.containsKey(accblk)) {
	     if(bindr.size() < realpages){
		 for (int i = 0; i < realpages; i++) //se tiver procura
		     if(!bindr.containsKey(i)){ //encontrou 
			 Mtotal.set(i*s, (i+1)*s); //preenche
			 binding (accblk, i); //faz a associação
			 CList.add(accblk); //entra na lista
			 pagebit[accblk] = true; //liga o bit de acesso
		     }
	     }
	     
	     else {
		 buffer = pointer;
		 while (pagebit[CList.get(pointer)]) { //olha o bit
		     pointer = (pointer + 1) % CList.size(); //é circular
		     if (buffer == pointer) break; //não entra em loop infinito
		 }
		 binding (accblk, bindv.get(CList.remove(pointer))); //associa
	     }	 
	}
	else pagebit[accblk] = true;
    }

    //////////////////////// PRINTS /////////////////////////////////

    //inicializa uma das memórias com -1
    public void fprintInicial (String nome, int tamanho) {
        try {
            RandomAccessFile file = new RandomAccessFile(nome, "rw");
            int numero = -1;
            for (int i = 0; i < tamanho; i++)
                file.writeInt(numero);
            file.close();
            System.out.println("Arquivo " + nome + " inicializado com " + numero);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    //inicializa uma das memórias com -1
    public void fprintPagina (String nome, int pid, int pagina, int tamanhopag) {
        try {
            RandomAccessFile file = new RandomAccessFile(nome, "rw");
            file.seek(tamanhopag*pagina);
            for (int i = 0; i < tamanhopag; i++)
                file.writeInt(pid);
            file.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    //printa o arquivo de caminho nome
    public void printArquivo (String nome) {
        byte bytef;
        try {
            RandomAccessFile file = new RandomAccessFile(nome, "r");
            for (int i = 0; i < file.length(); i++) {
                bytef = file.readByte();
                System.out.print(String.format("%8s", Integer.toBinaryString(bytef)).replace(' ', '0'));
                System.out.print(" ");
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void imprimir () {
        System.out.println("\nMemória Virtual");
        printArquivo("/tmp/ep3.vir");
        System.out.println("\nBitmap da memória virtual");
        printBitSet(Mvirtual);
        System.out.println("\nMemória física");
        printArquivo("/tmp/ep3.mem");
        System.out.println("\nBitmap da memória física");
        printBitSet(Mtotal);
    }

    //printa o estado da memória virtual
    public void printBitSet(BitSet memoria) {
        for (int i = 0; i < memoria.length(); i++) {
            if (memoria.get(i))
                for (int j = 0; j < s*p; j++)
                    System.out.print("1");
            else
                for (int j = 0; j < s*p; j++)
                    System.out.print("0");
        }
        System.out.print("\n");
    }

    //////////////////////////////// executar /////////////////////////////////

    //clase interna criada para ordenar eventos em função do tempo

    public class Cell implements Comparable<Cell> {
        public final Processo proc;
        public final double tempo;
        public final int pos;	//se evento for 0 ou 2 vale 0
        public final int tipoevento; //0 - inicializa, 1 - acesso, 2 - destroi

        public Cell(Processo proc, double tempo, int pos, int tipoevento) {
            this.proc = proc;
            this.tempo = tempo;
            if (tipoevento == 0 || tipoevento == 2)
            	this.pos = 0;
            else
            	this.pos = pos;
            this.tipoevento = tipoevento;
        }

       
        public int compareTo(Cell other) {
            //multiplied to -1 as the author need descending sort order
            return Double.valueOf(this.tempo).compareTo(other.tempo);
        }
    }

    public void executar () {
    	eventos = new ArrayList<Cell>();
	System.out.println(fila.peek().t0);
    	for (Processo proc : this.fila) {
	    eventos.add(new Cell (proc, proc.t0, 0, 0));
    		for (int i = 0; i < proc.posacessos.length; i++)
		    eventos.add(new Cell (proc, proc.tempacessos[i], proc.posacessos[i], 1));
    		eventos.add(new Cell (proc, proc.tf, 0, 2));
    	}
	
    	Collections.sort(eventos); //na teoria ta ordenado por tempo
    	double tpassado = 0;
    	for (Cell celula : eventos) {
    		while (tpassado < celula.tempo) {
    			imprimir();
    			tpassado += dt;
    		}
		
    		switch (celula.tipoevento) {
    			case 0:
    				//criar processo na memoria
    				switch (algespaco) {
    					case 1:
					        System.out.println("firstfit");
    						firstFit(celula.proc);
    						break;
    					case 2:
					        System.out.println("nextfit");
    						nextFit(celula.proc);
    						break;
    					case 3:
					        System.out.println("bestfit");
    						bestFit(celula.proc);
    						break;
    					case 4:
					        System.out.println("worstfit");
    						worstFit(celula.proc);
    						break;
    					default:
    						System.out.println("ALGO DEU ERRADO no switch algespaco");
    						break;
    				} 
    				break;
    			case 1:
    				//acessar endereço na memoria
    				switch (algsubstitui) {
    					case 1:
					        System.out.println("optimal");
    						optimal(celula.pos + celula.proc.offset);
    						break;
    					case 2:
					        System.out.println("SecondChance");
					        secondChance(celula.pos + celula.proc.offset);
    						break;
    					case 3:
					        System.out.println("Clock");
					        clock(celula.pos + celula.proc.offset);
    						break;
    					case 4:
    						//lru()
    						break;
    					default:
    						System.out.println("ALGO DEU ERRADO no switch algsubstitui");
    						break;
    				}
    				break;
    			case 2:
    				//remover(Proc)
    				//remover processo das memorias
    				break;
    			default:
    				System.out.println("ALGO DEU ERRADO no switch celula.tipoevento");
    				//na teoria nunca deveria passar
    				break;
    		}


    	}
    	    		System.out.println("Estado final");
    		imprimir();
    }
    
	
    // test client
    public static void main(String[] args) {
        Gerente gerente = new Gerente();
        while (true) {
            System.out.print("(ep3): ");
            String comando = StdIn.readString();
            if (comando.equals("carrega")) {
                gerente.carregarArquivo (StdIn.readString());
            } else if (comando.equals("espaco")) {
                gerente.algespaco = StdIn.readInt();
                System.out.println(gerente.algespaco);
            } else if (comando.equals("substitui")) {
                gerente.algsubstitui = StdIn.readInt();
                System.out.println(gerente.algsubstitui);
            } else if (comando.equals("executa")) {
                gerente.dt = StdIn.readDouble();
		gerente.executar();
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
