use std::{
    env::args,
    fs::File,
    thread,
    time::{Duration, SystemTime},
};

use anyhow::Result;
use chrono::{Datelike, Utc};

fn main() -> Result<()> {
    let start_time = Utc::now();
    let port = args().nth(1).expect("Please provide a port");
    let mut port = serialport::new(port, 9600)
        .timeout(Duration::from_secs(10))
        .open()
        .expect("Failed to open port");
    port.write_all(&[0x16])?; // SYN
    let mut buf = [0];
    loop {
        port.read_exact(&mut buf)?; // ACK
        if buf == [0x06] {
            break;
        }
        thread::yield_now();
        thread::sleep(Duration::from_millis(1));
    }
    let mut values = Vec::new();
    loop {
        match port.read_exact(&mut buf) {
            Err(err) if err.kind() == std::io::ErrorKind::BrokenPipe => break,
            Err(err) => {
                eprintln!("Error: {err}");
                break;
            }
            _ => (),
        }
        let time = SystemTime::now();
        values.push((time, buf[0]));
    }
    let mut file = File::create(format!(
        "data-{}-{}-{}-{}.mpk",
        start_time.year(),
        start_time.month(),
        start_time.day(),
        start_time.timestamp()
    ))?;
    rmp_serde::encode::write(&mut file, &values)?;
    Ok(())
}
