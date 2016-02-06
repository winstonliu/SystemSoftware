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
			SleepingBarber ()
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
		NUM_CUTS_EA = 1
	var
		barber_mon: BarberMonitor
		barbers: array [NUM_BARBERS] of Thread = new array of Thread {NUM_BARBERS of new Thread }
		customers: array [NUM_CUSTOMERS] of Thread = new array of Thread {NUM_CUSTOMERS of new Thread }

	function SleepingBarber ()
			var 
				i: int
				mystr: ptr to array of char = " "

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

			for i = 0 to NUM_BARBERS - 1
				-- Barber opens shop
				mystr[0] = intToChar (i)
				barbers[i].Init(mystr)
				barbers[i].Fork(OpenStore, i)
			endFor

			for i = NUM_BARBERS to (NUM_CUSTOMERS + NUM_BARBERS - 1) -- for ID purpose
				-- Customers arrive
				mystr[0] = intToChar (i)
				customers[i-NUM_BARBERS].Init(mystr)
				customers[i-NUM_BARBERS].Fork(GetHaircut, i)
			endFor
				
		endFunction

	function OpenStore (p:int)
			var
				k: int
			for k = 1 to NUM_BARBERS
				barber_mon.Barber(p)
			endFor
		endFunction

	function GetHaircut (p: int)
			-- Loops customers if they want to get multiple haircuts
			var
				k: int
			for k = 1 to NUM_CUTS_EA 
				barber_mon.Customer(p)
			endFor
		endFunction	

	class BarberMonitor
		superclass Object
		fields
			varLock: Mutex
			sem_customer, sem_barber: Semaphore	
			waiting: int
			id: int
		methods
			Init() -- id of thread
			Barber(p: int)	
			Customer(p: int)
			PrintStatus(status: int)
	endClass

	behavior BarberMonitor
		method Init()
				varLock = new Mutex
				varLock.Init()
				waiting = 0
				
				sem_barber = new Semaphore
				sem_customer = new Semaphore

				sem_customer.Init(0)
				sem_barber.Init(0)
			endMethod

		method Barber(p: int)
				while true
					sem_customer.Down()
					varLock.Lock()
					id = p
					-- indicates completion of previous cut
					if waiting > 0
						self.PrintStatus(END)
					endIf
					self.PrintStatus(START)
					waiting = waiting - 1
					varLock.Unlock()	
					sem_customer.Up()
					currentThread.Yield () -- cut hair
				endWhile
			endMethod

		method Customer(p: int)
				varLock.Lock()
				id = p
				self.PrintStatus(ENTER)
				if waiting < CHAIRS
					self.PrintStatus(SIT)
					waiting = waiting + 1
					sem_customer.Up()
					self.PrintStatus(BEGIN)
					varLock.Unlock() -- can't sleep holding the lock
					sem_barber.Down()
					-- reacquire lock to print out status
					varLock.Lock()
					self.PrintStatus(FINISH)
					varLock.Unlock()
				else
					self.PrintStatus(LEAVE)
					varLock.Unlock()
				endIf
			endMethod

		method PrintStatus(status: int)
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

				if id < NUM_BARBERS	-- thread is a Barber
					switch status
						case START:
							print ("STRT")
							break
						case END:
							print ("END")
							break
					endSwitch
				else
					print ("    ")
				endIf

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

endCode
