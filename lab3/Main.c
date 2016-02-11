code Main

  -- OS Class: Project 3
  --
  -- Chengzhi Winston Liu
  --

  -- This package contains the following:
  --     Dining Philospohers
-----------------------------  Main  ---------------------------------

  function main ()
      InitializeScheduler ()
      -- DiningPhilosophers ()
			-- SleepingBarber ()
			GamingParlor()
			ThreadFinish()	
    endFunction

-----------------------------  Dining Philosophers  ----------------------------

  -- This code is an implementation of the Dining Philosophers problem.  Each
  -- philosopher is simulated with a thread.  Each philosopher thinks for a while
  -- and then wants to eat.  Before eating, he must pick up both his forks.
  -- After eating, he puts down his forks.  Each fork is shared between
  -- two philosophers and there are 5 philosophers and 5 forks arranged in a
  -- circle.
  --
  -- Since the forks are shared, access to them is controlled by a monitor
  -- called "ForkMonitor".  The monitor is an object with two "entry" methods:
  --     PickupForks (phil)
  --     PutDownForks (phil)
  -- The philsophers are numbered 0 to 4 and each of these methods is passed an 
  -- integer indicating which philospher wants to pickup (or put down) the forks.
  -- The call to "PickUpForks" will wait until both of his forks are
  -- available.  The call to "PutDownForks" will never wait and may also
  -- wake up threads (i.e., philosophers) who are waiting.
  --
  -- Each philospher is in exactly one state: HUNGRY, EATING, or THINKING.  Each
  -- time a philosopher's state changes, a line of output is printed.  The 
  -- output is organized so that each philosopher has column of output with the
  -- following code letters:
  --           E    --  eating
  --           .    --  thinking
  --         blank  --  hungry (i.e., waiting for forks)
  -- By reading down a column, you can see the history of a philosopher.
  --
  -- The forks are not modeled explicitly.  A fork is only picked up
  -- by a philosopher if he can pick up both forks at the same time and begin
  -- eating.  To know whether a fork is available, it is sufficient to simply
  -- look at the status's of the two adjacent philosophers.  (Another way to 
  -- state the problem is to forget about the forks altogether and stipulate 
  -- that a philosopher may only eat when his two neighbors are not eating.)

  enum HUNGRY, EATING, THINKING
  var
    mon: ForkMonitor
    philospher: array [5] of Thread = new array of Thread {5 of new Thread }

  function DiningPhilosophers ()

      print ("Plato\n")
      print ("    Sartre\n")
      print ("        Kant\n")
      print ("            Nietzsche\n")
      print ("                Aristotle\n")

      mon = new ForkMonitor
      mon.Init ()
      mon.PrintAllStatus ()

      philospher[0].Init ("Plato")
      philospher[0].Fork (PhilosphizeAndEat, 0)

      philospher[1].Init ("Sartre")
      philospher[1].Fork (PhilosphizeAndEat, 1)

      philospher[2].Init ("Kant")
      philospher[2].Fork (PhilosphizeAndEat, 2)

      philospher[3].Init ("Nietzsche")
      philospher[3].Fork (PhilosphizeAndEat, 3)

      philospher[4].Init ("Aristotle")
      philospher[4].Fork (PhilosphizeAndEat, 4)

     endFunction

  function PhilosphizeAndEat (p: int)
    -- The parameter "p" identifies which philosopher this is.
    -- In a loop, he will think, acquire his forks, eat, and
    -- put down his forks.
      var
        i: int
      for i = 1 to 7
        -- Now he is thinking
        mon.PickupForks (p)
        -- Now he is eating
        mon.PutDownForks (p)
      endFor
    endFunction

  class ForkMonitor
    superclass Object
    fields
      status: array [5] of int -- For each philosopher: HUNGRY, EATING, or THINKING
			forkLock: Mutex
			forkCond: array [5] of Condition
    methods
      Init ()
      PickupForks (p: int)
      PutDownForks (p: int)
      PrintAllStatus ()
			left (p: int) returns int
			right (p: int) returns int 
  endClass

  behavior ForkMonitor

    method Init ()
				var i: int
				forkLock = new Mutex
				forkLock.Init()
				
				-- Initialize so that all philosophers are THINKING.
				status = new array of int {5 of THINKING}
				forkCond = new array of Condition {5 of new Condition}

				-- Initialize forkCond
				for i = 0 to 4
					forkCond[i].Init()
				endFor
			
      endMethod

    method PickupForks (p: int)
				-- This method is called when philosopher 'p' wants to eat.
				forkLock.Lock()
				status[p] = HUNGRY
				
				while status[self.left(p)] == EATING || status[self.right(p)] == EATING
					forkCond[p].Wait(&forkLock)
				endWhile

				status[p] = EATING

				self.PrintAllStatus()
				forkLock.Unlock()
      endMethod

    method PutDownForks (p: int)
				-- This method is called when the philosopher 'p' is done eating.
				forkLock.Lock()
				status[p] = THINKING

				if status[self.left(p)] == HUNGRY
					forkCond[self.left(p)].Signal(&forkLock)
				endIf

				if status[self.right(p)] == HUNGRY
					forkCond[self.right(p)].Signal(&forkLock)
				endIf

				self.PrintAllStatus()
				forkLock.Unlock()
      endMethod

    method PrintAllStatus ()
      -- Print a single line showing the status of all philosophers.
      --      '.' means thinking
      --      ' ' means hungry
      --      'E' means eating
      -- Note that this method is internal to the monitor.  Thus, when
      -- it is called, the monitor lock will already have been acquired
      -- by the thread.  Therefore, this method can never be re-entered,
      -- since only one thread at a time may execute within the monitor.
      -- Consequently, printing is safe.  This method calls the "print"
      -- routine several times to print a single line, but these will all
      -- happen without interuption.
        var
          p: int
        for p = 0 to 4
          switch status [p]
            case HUNGRY:
              print ("    ")
              break
            case EATING:
              print ("E   ")
              break
            case THINKING:
              print (".   ")
              break
          endSwitch
        endFor
        nl ()
      endMethod

		method left (p: int) returns int
				return (p + 4) % 5
			endMethod

		method right (p: int) returns int
				return (p + 1) % 5
			endMethod
			

  endBehavior

-----------------------------  Sleeping Barber Problem ----------------------------
	enum NONE, ENTER, SIT, BEGIN, FINISH, LEAVE, START, END
	const
		CHAIRS = 5 
		NUM_CUSTOMERS = 10
		NUM_BARBERS = 1
		NUM_CUTS_EA = 2
	var
		barber_mon: BarberMonitor
		barbers: array [NUM_BARBERS] of Thread = new array of Thread {NUM_BARBERS of new Thread }
		customers: array [NUM_CUSTOMERS] of Thread = new array of Thread {NUM_CUSTOMERS of new Thread }

	function SleepingBarber ()
			var 
				i: int

			for i = 1 to CHAIRS
				print (" ")
			endFor

			print (" BigB ")

			for i = 1 to NUM_CUSTOMERS
				printInt (i)
				print (" ")
			endFor
			
			nl()

			barber_mon = new BarberMonitor
			barber_mon.Init()

			-- Barber opens shop
			barbers[0].Init("B1")
			barbers[0].Fork(OpenStore, 0)

			-- Customers arrive
			customers[0].Init("C0")
			customers[0].Fork(GetHaircut, 1)

			customers[1].Init("C1")
			customers[1].Fork(GetHaircut, 2)

			customers[2].Init("C2")
			customers[2].Fork(GetHaircut, 3)
			
			customers[3].Init("C3")
			customers[3].Fork(GetHaircut, 4)

			customers[4].Init("C4")
			customers[4].Fork(GetHaircut, 5)

			customers[5].Init("C5")
			customers[5].Fork(GetHaircut, 6)

			customers[6].Init("C6")
			customers[6].Fork(GetHaircut, 7)

			customers[7].Init("C7")
			customers[7].Fork(GetHaircut, 8)

			customers[8].Init("C8")
			customers[8].Fork(GetHaircut, 9)

			customers[9].Init("C9")
			customers[9].Fork(GetHaircut, 10)

		endFunction

	function OpenStore (p:int)
			barber_mon.Barber(p)
		endFunction

	function GetHaircut (p: int)
			var
				k: int
			for k = 1 to NUM_CUTS_EA
				barber_mon.Customer(p)
			endFor
		endFunction	

	class BarberMonitor
		superclass Object
		fields
			bLock: Mutex
			sem_customer, sem_barber, sem_barber_done, sem_cLeave: Semaphore	
			waiting: int
			cust_cnt: int
		methods
			Init() -- id of thread
			Barber(p: int)	
			Customer(p: int)
			PrintStatus(status: int, id: int)
	endClass

	behavior BarberMonitor
		method Init()
				bLock = new Mutex
				bLock.Init()
				waiting = 0
				
				-- Counts the number of customers that have visited
				cust_cnt = 0
				
				sem_barber = new Semaphore
				sem_barber_done = new Semaphore
				sem_customer = new Semaphore
				sem_cLeave = new Semaphore

				sem_customer.Init(0)
				sem_barber.Init(0)
				sem_barber_done.Init(0)
				sem_cLeave.Init(0)
			endMethod

		method Barber(p: int)
				bLock.Lock()
				while cust_cnt <= NUM_CUSTOMERS * NUM_CUTS_EA
					bLock.Unlock()

					-- Go to sleep if no customers
					sem_customer.Down()

					bLock.Lock()
					waiting = waiting - 1
					self.PrintStatus(START, p)
					bLock.Unlock()	

					sem_barber.Up()
					currentThread.Yield () -- cut hair
					sem_barber_done.Down()

					bLock.Lock()
					self.PrintStatus(END, p)
					bLock.Unlock()

					sem_cLeave.Up()

					bLock.Lock()
				endWhile
			endMethod

		method Customer(p: int)
				bLock.Lock()
				cust_cnt = cust_cnt+1
				self.PrintStatus(ENTER, p)
				if waiting < CHAIRS
					waiting = waiting + 1
					self.PrintStatus(SIT, p)
					bLock.Unlock() -- can't sleep holding the lock

					sem_customer.Up()
					sem_barber.Down()

					bLock.Lock()
					self.PrintStatus(BEGIN, p)
					bLock.Unlock()

					currentThread.Yield () -- get hair cut

					-- reacquire lock to print out status
					bLock.Lock()
					self.PrintStatus(FINISH, p)
					bLock.Unlock()

					sem_barber_done.Up()
					sem_cLeave.Down()
				else
					bLock.Unlock()
				endIf
				bLock.Lock()
				self.PrintStatus(LEAVE, p)
				bLock.Unlock()
			endMethod

		method PrintStatus(status: int, id: int)
				-- USE ONLY FROM WITHIN MUTEX BLOCK
				var
					k: int

				-- Print occupado status
				for k = 1 to CHAIRS
					if waiting < k
						print ("-")
					else
						print ("X")
					endIf
				endFor

				print (" ")

				switch status
					case START:
						print ("STRT")
						break
					case END:
						print ("END")
						break
					default:
						print ("    ")
						break
				endSwitch
				
				for k = 1 to NUM_CUSTOMERS
					if id == k
						switch status
							case ENTER:
								print (" E")
								break
							case SIT:
								print (" S")
								break
							case BEGIN:
								print (" B")
								break
							case FINISH:
								print (" F")
								break
							case LEAVE:
								print (" L")
								break
						endSwitch
					else
						print ("  ")
					endIf
				endFor
				nl()
			endMethod	

	endBehavior

----------------------------- Gaming Parlor Problem ----------------------------

	const
		NUM_PLAYS = 5
		NUM_DICE = 8
		NUM_GAMES = 8
  var
    pmon: ParlorMonitor
    games: array [NUM_GAMES] of Thread = new array of Thread {NUM_GAMES of new Thread }

	function GamingParlor()
			pmon = new ParlorMonitor	
			pmon.Init()

			games[0].Init("A")
			games[0].Fork(PlayGames, 4)

			games[1].Init("B")
			games[1].Fork(PlayGames, 4)

			games[2].Init("C")
			games[2].Fork(PlayGames, 5)		

			games[3].Init("D")
			games[3].Fork(PlayGames, 5)		

			games[4].Init("E")
			games[4].Fork(PlayGames, 2)		

			games[5].Init("F")
			games[5].Fork(PlayGames, 2)		

			games[6].Init("G")
			games[6].Fork(PlayGames, 1)		

			games[7].Init("H")
			games[7].Fork(PlayGames, 1)		
		endFunction

	function PlayGames(p:int)
			var
				k: int
			for k = 1 to NUM_PLAYS
				pmon.Request(p)
				pmon.Return(p)
			endFor
		endFunction

	class ParlorMonitor
		superclass Object
		fields
			numberDiceAvail: int
			pLock: Mutex
			pCond: Condition
			pCondCnt: int
		methods
			Init()
			Request(numDice: int)
			Return(numDice: int)
			Print(str: String, count: int)

	endClass

	behavior ParlorMonitor
		method Init()
				numberDiceAvail = NUM_DICE 

				pLock = new Mutex
				pLock.Init()

				pCond = new Condition
				pCond.Init()
			endMethod

		method Request(numDice: int)
				pLock.Lock()	
				self.Print("requests", numDice)
				while numberDiceAvail < numDice
					pCond.Wait(&pLock)
				endWhile
				numberDiceAvail = numberDiceAvail - numDice
				self.Print("proceeds with", numDice)
				pLock.Unlock()
			endMethod
 
		method Return(numDice: int)
				pLock.Lock()
				self.Print("releases and adds back", numDice)
				pCond.Signal(&pLock)
				numberDiceAvail = numberDiceAvail + numDice
				pLock.Unlock()
			endMethod

		method Print(str: String, count: int)
				print (currentThread.name)
				print (" ")
				print (str)
				print (" ")
				printInt (count)
				nl ()
				print ("------------------------------Number of dice now avail = ")
				printInt (numberDiceAvail)
				nl()
			endMethod
	
	endBehavior


endCode
