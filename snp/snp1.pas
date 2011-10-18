program snp1;

{$APPTYPE CONSOLE}
{$R+}
uses
  SysUtils,
  Classes,
  UnitMesUtils_5 in 'UnitMesUtils_5.pas';
 
const
  ah:array[0..2]of extended =(0.2162,0.6102,0.3046);
  bh:array[0..2]of extended =(0.4690,0.3842,0.3753);
  af:array[0..2]of extended =(0.5246,0.3389,0.4672);
  bf:array[0..2]of extended =(1.6196,1.2259,2.4070);
  nbr=10;
  anbr=1.0/nbr;

type
  arrayS =array of string;

var
  gen  :array of array of array of integer;
  geno :array of array of smallint;
  samplesize : array of array of integer;
  indivsexe,indivpop :array of integer;
  indivname :array of string;  
  freq :array of array of extended;
  nloc,npop,nloc0,nloc1,rep  :integer;
  nind       :array of integer;
  nindtot    :integer;
  popname    :arrayS;
  t0         :TDateTime;
  myHour, myMin, mySec, myMilli : Word;
  pobth,pobtf,pobtn       :array[0..2,0..pred(nbr)] of extended;
  quanth,quantf,quantn    :array[0..2,0..pred(nbr)] of extended;
  DD         :array[0..2,0..3,0..2,0..7] of extended;
  f2  :textfile;

procedure splitwords(s:string;sep:string;var nss:integer;var ss:arrayS;lim:integer);
  var
    i,j,j0,j1   :integer;
    s0,s1     :string;
  begin
	j:=0;
	while pos(sep,s)=1 do s:=copy(s,2,length(s));
	nss:=0;
	s:=s+sep;
	s1:='';
	for i:=1 to length(s) do
	  begin
	    s0:=copy(s,i,1);
	    if s0=sep then	
	      begin
	        inc(j);
	        if j=1 then
	          begin
	            s1:=s1+s0;
	            if j=1 then inc(nss);
	            if (lim>0)and(nss=lim) then break;
	          end;
	      end
		  else
		  begin
			s1:=s1+s0;
			j:=0;
		  end;
	  end;
	setlength(ss,nss);
	for i:=0 to pred(nss) do
	  begin
		j0:=pos(sep,s1);
		ss[i]:=copy(s1,1,pred(j0));
		s1:=copy(s1,succ(j0),length(s1));
	  end;
  end;

procedure readfile;
  var
	f0  :textfile;
	i,j,k,nss,io,nli,nl  :integer;
	ligne,pop  :string;
	ss     :arrayS;
	trouve  :boolean;
  begin
	assignfile(f0,'geno_A.txt');reset(f0);
	readln(f0,ligne);
	nloc:=0;
	ligne:=copy(ligne,pos('POP',ligne)+3,length(ligne));
	nloc:=length(ligne) div 2;
    writeln('nloc=',nloc);
    splitwords(ligne,' ',nss,ss,-1);
    writeln('nss=',nss);
    npop:=1;setlength(popname,1);setlength(nind,1);
    readln(f0,ligne);
    splitwords(ligne,' ',nss,ss,3);
	popname[pred(npop)]:=ss[2];
	nind[pred(npop)]:=1;nli:=0;
	while not eof(f0) do
	  begin
		readln(f0,ligne);inc(nli);
		splitwords(ligne,' ',nss,ss,3);
		j:=0;trouve:=false;
	    while (j<npop)and(not trouve) do
	      begin
	        trouve:= ss[2]=popname[j];
	        if not trouve then inc(j);
	      end;
	    if trouve then inc(nind[j]) else
	      begin
			inc(npop);setlength(popname,npop);setlength(nind,npop);
			nind[pred(npop)]:=1;
			popname[pred(npop)]:=ss[2];
	      end;
	      
	  end;
    writeln('npop = ',npop);
    for i:=0 to pred(npop) do write(popname[i],'  (',nind[i],')   ');writeln;
	setlength(gen,npop);for i:=0 to pred(npop) do setlength(gen[i],nind[i],nloc);
	nindtot:=succ(nli);
	writeln('nindtot=',nindtot);
	setlength(indivname,nindtot);
	setlength(indivsexe,nindtot);
	setlength(indivpop,nindtot);
	setlength(geno,nindtot,nloc);
	reset(f0);
	readln(f0,ligne);nl:=0;
	for i:=0 to pred(npop) do
	  begin
		for j:=0 to pred(nind[i]) do
		  begin
			readln(f0,ligne);inc(nl);writeln('nl=',nl,'   ',copy(ligne,1,20));
			splitwords(ligne,' ',nss,ss,-1);
			//writeln('lecture de ',ss[0],' ',ss[2]);
			if nss<>nloc+3 then begin writeln('ligne incorrecte');halt;end;
			indivname[pred(nl)]:=ss[0];
			indivsexe[pred(nl)]:=0;
			if ss[1]='M' then indivsexe[pred(nl)]:=1;
			if ss[1]='F' then indivsexe[pred(nl)]:=2;
			k:=0;while (ss[2]<>popname[k]) do inc(k);
			indivpop[pred(nl)]:=k;
		    for k:=0 to pred(nloc) do begin val(ss[k+3],gen[i][j][k],io);geno[pred(nl),k]:=gen[i][j][k];end;
		    //write(chr(13),'lecture du fichier ',100.0*nl/nli:4:0,'%');
		  end;
	  end;
	writeln;
	if nloc1>nloc then nloc1:=nloc;
	writeln('nloc limité à ',nloc,'  nloc0=',nloc0,'  nloc1=',nloc1);
  end;

{procedure ecribin;
  var
	i,j,k,pop,ind,long :integer;
	fi :TFilestream;
	ss :AnsiString;
  begin
    fi:=TFilestream.Create('geno_A.bin',fmCreate);
    fi.WriteBuffer(nloc,4);
    ss:='A';
    for i:=0 to pred(nloc) do fi.WriteBuffer(Pchar(ss)^,1);
    fi.WriteBuffer(npop,4);
    for i:=0 to pred(npop) do 
	  begin 
		ss:=popname[i];
		long:=length(ss);
		fi.WriteBuffer(long,4);
		fi.WriteBuffer(Pchar(ss)^,long);
	  end;
    for i:=0 to pred(npop) do fi.WriteBuffer(nind[i],4);
    for pop:=0 to pred(npop) do
      begin
		for ind:=0 to pred(nindtot) do
		  begin
			if (indivpop[ind]=pop) then 
			  begin
				fi.WriteBuffer(pop,4);
				fi.WriteBuffer(indivsexe[ind],4);
				ss:=indivname[ind];
				long:=length(ss);
				fi.WriteBuffer(long,4);
				fi.WriteBuffer(Pchar(ss)^,long);
				k:=ind;j:=0;
				for i:=0 to pred(nloc) do fi.WriteBuffer(geno[ind][i],2);
			  end;
		  end;
	  end;
	fi.Destroy;
  end;

procedure libin;
  var
	i,j,k,pop,ind,long :integer;
	fi :TfileStream;
	ss :AnsiString;
	c  :array of ansichar;
  begin
	fi:=TFileStream.Create('geno_A.bin',fmOpenRead);
	fi.ReadBuffer(nloc,4);writeln('nloc=',nloc);
	setlength(c,nloc);writeln('apres setlength');
	fi.Read(c,2);writeln('apres fi.read');
	write(ord(c[0]),' ',c[1]);writeln;
	halt;
	fi.ReadBuffer(npop,4);writeln('npop=',npop);
  end;}

procedure calfreq;
  var
    i,j,k,n  :integer;
    s :extended;
    
  begin
    setlength(freq,npop);for i:=0 to pred(npop) do setlength(freq[i],nloc);
    setlength(samplesize,npop);for i:=0 to pred(npop) do setlength(samplesize[i],nloc);
    
    for i:=0 to pred(npop) do
      begin
        for k:=nloc0 to pred(nloc1) do
          begin
            n:=0;s:=0.0;
            for j:=0 to pred(nind[i]) do if gen[i][j][k]<>9 then 
			  begin 
				n:=n+2;
				s:=s+gen[i][j][k];
			  end;
            if n>0 then freq[i][k]:=s/n else freq[i][k]:=0.0;
            samplesize[i][k]:=n;
          end;
          
      end;
  end;

procedure calhet(tout:boolean);
  var
    i,k,nl,j,loc,q  :integer;
    hetmoy,hetvar,sx,sx2,a,b,he  :array of extended;
    f1  :textfile;
    locOK :array of array of boolean;
    het  :array of array of extended;
	pob,quant :array of extended;
	sw,Dkl,x,Dabs,Deuc,Dqua  :extended;
  begin
    setlength(het,npop);for i:=0 to pred(npop) do setlength(het[i],nloc);setlength(he,nloc1-nloc0);
    setlength(hetmoy,npop);setlength(hetvar,npop);setlength(sx,npop);setlength(sx2,npop);setlength(a,npop);setlength(b,npop);
    setlength(locOK,npop,nloc);
    setlength(pob,nbr);setlength(quant,nbr);
    for k:=0 to pred(npop) do for loc:=nloc0 to pred(nloc1) do locOK[k][loc]:=true;
    for k:=0 to pred(npop) do
      begin
        sx[k]:=0.0;sx2[k]:=0.0;nl:=0;for i:=0 to pred(nbr) do pob[i]:=0.0;
        for loc:=nloc0 to pred(nloc1) do
          begin
            het[k][loc]:=0.0;
            if samplesize[k][loc]>40 then
              begin
                inc(nl);locOK[k][loc]:=true;
				het[k][loc]:=2.0*(1.0 - sqr(freq[k][loc])-sqr(1.0-freq[k][loc])){*samplesize[k][loc]/pred(samplesize[k][loc])};
				sx[k]:=sx[k]+het[k][loc];
				sx2[k]:=sx2[k]+sqr(het[k][loc]);
				i:=trunc(nbr*het[k][loc]);
				if (i>pred(nbr)) then i:=pred(nbr);
				pob[i]:=pob[i]+1.0;
			  end else locOK[k][loc]:=false;
          end;
        for loc:=nloc0 to pred(nloc1) do he[loc-nloc0]:=het[k][loc];  
          writeln('avant fsort');
        fsort('a',nloc1-nloc0,he);
        writeln('apres fsort');
        for i:=0 to pred(nbr) do begin q:= succ(i)*(nloc1-nloc0) div succ(nbr);quant[i]:=he[q];end;
        if tout then write('TOUT ');
        write('Het (',k,')   ');
        for i:=0 to pred(nbr) do write(quant[i]:6:3);
        writeln;
		sw:=0.0;
		for i:=0 to pred(nbr) do sw:=sw+pob[i];
		for i:=0 to pred(nbr) do pob[i]:=pob[i]/sw;
		for i:=0 to pred(nbr) do write(pob[i]:6:3);writeln;
        Dkl:=0.0;Dabs:=0.0;Deuc:=0.0;Dqua:=0.0;
        if tout then for j:=0 to pred(nbr) do begin pobth[k][j]:=pob[j];quanth[k][j]:=quant[j];end else
          begin
            for i:=0 to pred(nbr) do if pobth[k][i]*pob[i]>0.0 then 
              begin
				x:=anbr*ln(pob[i]/pobth[k][i]);
				Dkl:=Dkl+0.5*(pob[i]*x-x*pobth[k][i]);
              end;
            for i:=0 to pred(nbr) do Dabs:=Dabs+abs(pob[i]-pobth[k][i]);Dabs:=anbr*Dabs;
            for i:=0 to pred(nbr) do Deuc:=Deuc+sqr(pob[i]-pobth[k][i]);Deuc:=anbr*sqrt(Deuc);
            for i:=0 to pred(nbr) do Dqua:=Dqua+sqr(quant[i]-quanth[k][i]);Dqua:=anbr*sqrt(Dqua);
          end;
        hetmoy[k]:=sx[k]/nl;
        hetvar[k]:=(sx2[k]-sqr(sx[k])/nl)/nl;
        write('hetmoy[',k,']=',hetmoy[k]:6:4,'     hetvar[',k,']=',hetvar[k]:12:6);
        a[k]:=(hetmoy[k]*(1.0-hetmoy[k]))/hetvar[k]-1.0; 
        b[k]:=(1.0-hetmoy[k])*a[k];
        a[k]:=hetmoy[k]*a[k];
        writeln('     a[',k,']=',a[k]:8:4,'   b[',k,']=',b[k]:8:4,'  dist=',sqrt(sqr(a[k]-ah[k])+sqr(b[k]-bh[k])):8:4,'  Dkl=',Dkl:10:6);
        if (not tout) then 
          begin
            writeln('avant write(,f2)');
            write(f2,sqrt(sqr(a[k]-ah[k])+sqr(b[k]-bh[k])):10:8,'     ',Dkl:10:8,'     ',Dabs:10:8,'     ',Deuc:10:8,'     ',Dqua:10:8);
            writeln(f2,'   H',k);
            DD[0,0,k,rep]:=Dkl;DD[0,1,k,rep]:=Dabs;DD[0,2,k,rep]:=Deuc;DD[0,3,k,rep]:=Dqua;
          end;
      end;
    assignfile(f1,'het.txt');rewrite(f1);
    for k:=0 to pred(npop) do
      begin
        for loc:=nloc0 to pred(nloc1) do if locOK[k][loc] then write(f1,het[k][loc]:7:4);writeln(f1);
      end;
    closefile(f1);
  end;
  
procedure NeiDist(tout:boolean);
  var
	i,j,k,nl,ipop,jpop,np,loc,all,q :integer;
	neimoy,neivar,sx,sx2,a,b,ne  :array of extended;
	Dkl,x,Dabs,Deuc,fi,fj,gi,gj,sw,Dqua :extended;
	f1  :textfile;
	locOK  :array of array of boolean;
	nei  :array of array of extended;
	pob,quant :array of extended;
  begin
    np:=npop*pred(npop) div 2;
    setlength(nei,np);for i:=0 to pred(np) do setlength(nei[i],nloc);setlength(ne,nloc1-nloc0);
    setlength(neimoy,npop);setlength(neivar,npop);setlength(sx,np);setlength(sx2,np);setlength(a,np);setlength(b,np);
    setlength(locOK,np,nloc);
    setlength(pob,nbr);setlength(quant,nbr);
    for i:=0 to pred(np) do for loc:=0 to pred(nloc) do locOK[i][loc]:=true;
    k:=-1;
	for ipop:=0 to npop-2 do
	  begin
		for jpop:=succ(ipop) to pred(npop) do
		  begin
			inc(k);
			sx[k]:=0.0;sx2[k]:=0.0;nl:=0;for i:=0 to pred(nbr) do pob[i]:=0.0;
			for loc:=nloc0 to pred(nloc1) do
			  begin
			    if (samplesize[ipop][loc]>40)and(samplesize[jpop][loc]>40) then
				  begin
					locOK[k][loc]:=true;inc(nl);
					fi:=freq[ipop][loc];fj:=freq[jpop][loc];gi:=1.0-fi;gj:=1.0-fj;
					nei[k][loc]:=(fi*fj+gi*gj)/sqrt(sqr(fi)+sqr(gi))/sqrt(sqr(fj)+sqr(gj));
				  end else locOK[k][loc]:=false;
			  end;	
			for loc:=nloc0 to pred(nloc1) do if locOK[k][loc] then 
			  begin 
				i:=trunc(nbr*nei[k][loc]);
				if (i>pred(nbr)) then i:=pred(nbr);
				pob[i]:=pob[i]+1.0;
			  end;
			sw:=0.0;for i:=0 to pred(nbr) do sw:=sw+pob[i];
			for i:=0 to pred(nbr) do pob[i]:=pob[i]/sw;
			for loc:=nloc0 to pred(nloc1) do ne[loc-nloc0]:=nei[k][loc];  
			fsort('a',nloc1-nloc0,ne);
			for i:=0 to pred(nbr) do begin q:= succ(i)*(nloc1-nloc0) div succ(nbr);quant[i]:=ne[q];end;
			if tout then write('TOUT ');
			write('Nei (',k,')   ');
			for i:=0 to pred(nbr) do write(quant[i]:6:3);
			writeln;
			Dkl:=0.0;Dabs:=0.0;Deuc:=0.0;Dqua:=0.0;
			if tout then for j:=0 to pred(nbr) do begin pobtn[k][j]:=pob[j];quantn[k][j]:=quant[j];end else
			  begin
				for i:=0 to pred(nbr) do if pobtn[k][i]*pob[i]>0.0 then 
				  begin
					x:=anbr*ln(pob[i]/pobtn[k][i]);
					Dkl:=Dkl+0.5*(pob[i]*x-x*pobtn[k][i]);
				  end;
				for i:=0 to pred(nbr) do Dabs:=Dabs+abs(pob[i]-pobtn[k][i]);Dabs:=anbr*Dabs;
				for i:=0 to pred(nbr) do Deuc:=Deuc+sqr(pob[i]-pobtn[k][i]);Deuc:=anbr*sqrt(Deuc);
				for i:=0 to pred(nbr) do Dqua:=Dqua+sqr(quant[i]-quantn[k][i]);Dqua:=anbr*sqrt(Dqua);
			  end;
		  for loc:=nloc0 to pred(nloc1) do if locOK[k][loc] then begin sx[k]:=sx[k]+nei[k][loc];sx2[k]:=sx2[k]+sqr(nei[k][loc]);end;
		  neimoy[k]:=sx[k]/nl;
		  neivar[k]:=(sx2[k]-sqr(sx[k])/nl)/nl;
		  write('neimoy[',k,']=',neimoy[k]:6:4,'     neivar[',k,']=',neivar[k]:12:6);
		  a[k]:=(neimoy[k]*(1.0-neimoy[k]))/neivar[k]-1.0; 
		  b[k]:=(1.0-neimoy[k])*a[k];
		  a[k]:=neimoy[k]*a[k];
		  writeln('     a[',k,']=',a[k]:8:4,'   b[',k,']=',b[k]:8:4,'  dist=',sqrt(sqr(a[k]-ah[k])+sqr(b[k]-bh[k])):8:4,'  Dabs=',Dabs:10:6);
		  if (not tout) then 
			begin
			  write(f2,sqrt(sqr(a[k]-ah[k])+sqr(b[k]-bh[k])):10:8,'     ',Dkl:10:8,'     ',Dabs:10:8,'     ',Deuc:10:8,'     ',Dqua:10:8);
			  writeln(f2,'   N',k);
              DD[1,0,k,rep]:=Dkl;DD[1,1,k,rep]:=Dabs;DD[1,2,k,rep]:=Deuc;DD[1,3,k,rep]:=Dqua;
			end;
		end;
    end;
  assignfile(f1,'nei.txt');rewrite(f1);
  for k:=0 to pred(np) do
    begin
      for loc:=nloc0 to pred(nloc1) do if locOK[k][loc] then write(f1,nei[k][loc]:7:4);writeln(f1);
    end;
  closefile(f1);
end;

procedure calFst(tout:boolean);
  var
	i,k,nl,ipop,jpop,np,loc,all,q :integer;
	fstmoy,fstvar,sx,sx2,a,b,fs  :array of extended;
	sniA,sniAA,sni,sni2,s2A,fi,fj,sw,sw2,a1,b1,Dkl,x,Dabs,Deuc,Dqua :extended;
	f1  :textfile;
	locOK  :array of array of boolean;
	fst,w  :array of array of extended;
	s1l,s3l,nc,MSI,MSP,s2I,s2P:extended;
	pob,quant :array of extended;
  begin
    np:=npop*pred(npop) div 2;
    setlength(fst,np);for i:=0 to pred(np) do setlength(fst[i],nloc);
    setlength(w,np);for i:=0 to pred(np) do setlength(w[i],nloc);
    setlength(fstmoy,npop);setlength(fstvar,npop);setlength(sx,np);setlength(sx2,np);setlength(a,np);setlength(b,np);
    setlength(locOK,np,nloc);
    setlength(pob,nbr);setlength(fs,nloc1-nloc0);setlength(quant,nbr);
    for i:=0 to pred(np) do for loc:=0 to pred(nloc) do locOK[i][loc]:=true;
    k:=-1;
	for ipop:=0 to npop-2 do
	  begin
		for jpop:=succ(ipop) to pred(npop) do
		  begin
			inc(k);
			sx[k]:=0.0;sx2[k]:=0.0;nl:=0;for i:=0 to pred(nbr) do pob[i]:=0.0;
			for loc:=nloc0 to pred(nloc1) do
			  begin
			    if (samplesize[ipop][loc]>40)and(samplesize[jpop][loc]>40) then
				  begin
					locOK[k][loc]:=true;inc(nl);
					s1l:=0.0;s3l:=0.0;
					for all:=0 to 1 do
					  begin
					    if all=0 then 
						  begin fi:=1.0-freq[ipop][loc];fj:=1.0-freq[jpop][loc];end 
						  else
						  begin fi:=freq[ipop][loc];fj:=freq[jpop][loc];end;
						sniAA:=fi*samplesize[ipop][loc]+fj*samplesize[jpop][loc];
						sniA:=2.0*sniAA;
						sni:=1.0*(samplesize[ipop][loc]+samplesize[jpop][loc]);
						sni2:=1.0*(sqr(samplesize[ipop][loc])+sqr(samplesize[jpop][loc]));
						s2A:=2.0*samplesize[ipop][loc]*sqr(fi)+2.0*samplesize[jpop][loc]*sqr(fj);
						nc:=1.0*(sni-sni2/sni);
						MSI:=(0.5*sniA+sniAA-s2A)/(sni-2);
						MSP:=(s2A-0.5*sqr(sniA)/sni);
						s2I:=0.5*MSI;
						s2P:=(MSP-MSI)/(2*nc);
						s1l:=s1l+s2P;
						s3l:=s3l+s2P+s2I;
					  end;
					if (s3l>0.0)and(s1l>0.0) then fst[k][loc]:=s1l/s3l else fst[k][loc]:=0.0;
					w[k][loc]:=1.0*nc;
				  end else locOK[k][loc]:=false;
			  end;
			//  writeln('FST fin de la premiere boucle sur les locus');
			for loc:=nloc0 to pred(nloc1) do fs[loc-nloc0]:=fst[k][loc];
			//writeln('2');
			fsort('a',nloc1-nloc0,fs);
			//writeln('3');
			for i:=0 to pred(nbr) do begin q:= succ(i)*(nloc1-nloc0) div succ(nbr);quant[i]:=fs[q];end;
			//writeln('4');
			if tout then write('TOUT ');
			write('Fst (',k,')   ');
			for i:=0 to pred(nbr) do write(quant[i]:6:3);
			writeln;
			  
			sw:=0.0;
			for loc:=nloc0 to pred(nloc1) do if locOK[k][loc] then sw:=sw+w[k][loc];
			for loc:=nloc0 to pred(nloc1) do if locOK[k][loc] then w[k][loc]:=w[k][loc]/sw;
			
			for loc:=nloc0 to pred(nloc1) do if locOK[k][loc] then 
			  begin 
				i:=trunc(nbr*fst[k][loc]);
				if (i>pred(nbr)) then i:=pred(nbr);
				pob[i]:=pob[i]+w[k][loc];
			  end;
			for i:=0 to pred(nbr)  do write(pob[i]:8:4);writeln;
			sw:=0.0;
			for i:=0 to pred(nbr) do sw:=sw+pob[i];
			writeln('somme des pob=',sw:6:3);
			sw2:=0.0;for loc:=nloc0 to pred(nloc1) do if locOK[k][loc] then sw2:=sw2+sqr(w[k][loc]);
			for loc:=nloc0 to pred(nloc1) do if locOK[k][loc] then begin sx[k]:=sx[k]+w[k][loc]*fst[k][loc];sx2[k]:=sx2[k]+w[k][loc]*sqr(fst[k][loc]);end;
			fstmoy[k]:=sx[k];
			fstvar[k]:=(sx2[k]-sqr(sx[k]))/(1.0-sw2);
			a[k]:=(fstmoy[k]*(1.0-fstmoy[k]))/fstvar[k]-1.0;
			b[k]:=(1.0-fstmoy[k])*a[k];
			a[k]:=fstmoy[k]*a[k];
			a1:=a[k]-1.0;b1:=b[k]-1.0;
			Dkl:=0.0;Dabs:=0.0;Deuc:=0.0;Dqua:=0.0;
			if tout then 
			  begin 
				for i:=0 to pred(nbr) do 
				  begin
					pobtf[k][i]:=pob[i];
					quantf[k][i]:=quant[i];
				  end;
			  end
			  else
			  begin
				for i:=0 to pred(nbr) do 
				  begin
					//writeln(pobtf[k][i]:12:6,pob[i]:12:6);
					if pobtf[k][i]*pob[i]>0.0 then 
					  begin
						x:=anbr*ln(pob[i]/pobtf[k][i]);
						Dkl:=Dkl+0.5*(pob[i]*x-x*pobtf[k][i]);
					  end;
				  end;
				for i:=0 to pred(nbr) do Dabs:=Dabs+abs(pob[i]-pobtf[k][i]);Dabs:=anbr*Dabs;
				for i:=0 to pred(nbr) do Deuc:=Deuc+sqr(pob[i]-pobtf[k][i]);Deuc:=anbr*sqrt(Deuc);
				for i:=0 to pred(nbr) do Dqua:=Dqua+sqr(quant[i]-quantf[k][i]);Dqua:=anbr*sqrt(Dqua);
			  end;
			write('fstmoy[',k,']=',fstmoy[k]:6:4,'     fstvar[',k,']=',fstvar[k]:12:6);
			writeln('     a[',k,']=',a[k]:8:4,'   b[',k,']=',b[k]:8:4,'  dist=',sqrt(sqr(a[k]-af[k])+sqr(b[k]-bf[k])):8:4,'  Dkl=',Dkl:10:6);
			if (not tout) then 
			  begin
			    write(f2,sqrt(sqr(a[k]-af[k])+sqr(b[k]-bf[k])):10:8,'     ',Dkl:10:8,'     ',Dabs:10:8,'     ',Deuc:10:8,'     ',Dqua:10:8);
			    writeln(f2,'   F',k);
                DD[2,0,k,rep]:=Dkl;DD[2,1,k,rep]:=Dabs;DD[2,2,k,rep]:=Deuc;DD[2,3,k,rep]:=Dqua;
			  end;
		end;
      end;
	assignfile(f1,'fst.txt');rewrite(f1);
    for k:=0 to pred(np) do
      begin
		assignfile(f1,'fst_'+inttostr(k)+'.txt');rewrite(f1);
        for loc:=nloc0 to pred(nloc1) do if locOK[k][loc] then write(f1,fst[k][loc]:7:4);writeln(f1);
		closefile(f1);      
	  end;
  end;
  
procedure compdist;
  var
    i,j,k,l :integer;
    sx,sx2,v,m :extended;
  begin
	for i:=0 to 2 do //SS
	  begin
	    writeln;
	    if i=0 then writeln('hétérozygotie');
	    if i=1 then writeln('Neis distance');
	    if i=2 then writeln('Fst distance');
	    for k:=0 to 2 do //np
	      begin
	        writeln('np=',k,' Kullback      Dabs         Deuc       Dqua');
	        for j:=0 to 3 do //dist
	          begin
				sx:=0.0;sx2:=0.0;
				for l:=0 to pred(8) do begin sx:=sx+DD[i,j,k,l];sx2:=sx2+sqr(DD[i,j,k,l]);end;
				v:=(sx2-sx*sx/8.0)/7.0;m:=sx/8.0;
				write(sqrt(v)/m:12:4);
			  end;
			  writeln;
	      end;
	  end;
  end;
  
begin
  if paramCount=2 then begin val(paramstr(1),nloc0);val(paramstr(2),nloc1);end else begin nloc0:=0;nloc1:=100000000;end;
  t0:=now;
  readfile;
  writeln('durée de la lecture: ',TimeToStr(now-t0));
  {t0:=now;
  writeln('avant ecribin');
  ecribin;
  t0:=now;
  libin;
  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
  writeln('durée de la lecture du bin: ',IntToStr(myMilli),' millisecondes');
  halt;}
  calfreq;
  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
  writeln('durée du calcul des fréquences: ',IntToStr(myMilli),' millisecondes'); 
  t0:=now;
  calhet(true);
  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
  writeln('durée du calcul des hétérozygoties: ',IntToStr(myMilli),' millisecondes');
  t0:=now;
  NeiDist(true);
  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
  writeln('durée du calcul des Nei: ',IntToStr(myMilli),' millisecondes');
  t0:=now;
  calFst(true);
  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
  writeln('durée du calcul des Fst: ',IntToStr(myMilli),' millisecondes');
  assignfile(f2,'comp_dist.txt');rewrite(f2);
  for rep:=0 to 7 do
    begin
      nloc0:=rep*5000;
      nloc1:=succ(rep)*5000;
      writeln('nloc0 = ',nloc0,'   nloc1 = ',nloc1);
	  t0:=now;
	  calhet(false);
	  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
	  writeln('durée du calcul des hétérozygoties: ',IntToStr(myMilli),' millisecondes');
	  t0:=now;
	  NeiDist(false);
	  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
	  writeln('durée du calcul des Nei: ',IntToStr(myMilli),' millisecondes');
	  t0:=now;
	  calFst(false);
	  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
	  writeln('durée du calcul des Fst: ',IntToStr(myMilli),' millisecondes');
    end;
   closefile(f2);
   compdist;
end.
  