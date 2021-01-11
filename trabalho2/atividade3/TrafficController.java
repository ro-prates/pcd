import java.util.ArrayDeque;
import java.util.Queue;
import java.util.logging.Level;
import java.util.logging.Logger;

public class TrafficController{
    
    Queue<Thread> fila_carros;


    public TrafficController() {
        fila_carros = new ArrayDeque<Thread>();
    }

    public synchronized void enterLeft() {
        fila_carros.add(Thread.currentThread());
        while (fila_carros.peek() != Thread.currentThread()){
            try {
                wait();
            } catch (InterruptedException ex) {
                Logger.getLogger(TrafficController.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }

    public synchronized void enterRight() {
        fila_carros.add(Thread.currentThread());


        while (fila_carros.peek() != Thread.currentThread()) {
            try {
                wait();
            } catch (InterruptedException ex) {
                Logger.getLogger(TrafficController.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }


    public synchronized void leaveLeft() {
        if (fila_carros.poll() != Thread.currentThread()) {
            System.err.println("Falha!");
        }
        notifyAll();
    }

    public synchronized void leaveRight() {
        if (fila_carros.poll() != Thread.currentThread()) {
            System.err.println("Falha!");
        }
        notifyAll();
    }
}