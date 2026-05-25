csv_path <- "metrics.csv"
if(!file.exists(csv_path)){
    stop("Error: metrics.csv not foud. Please run C++ validator.exe file first, then run this again")
}

metrics <- read.csv(csv_path)

metrics$latency_us <- metrics$latency_ns / 1000

total_ticks <- nrow(metrics)
anomalies <- sum(metrics$validation_flags != 0)
mean_lantecy <- mean(metrics$latency_us)
p95_latency <- quantile(metrics$latency_us, 0.95)
p99_latency <- quantile(metrics$latency_us, 0.99)
max_latency <- max(metrics$latency_us)

cat("==================================================\n")
cat("          PIPELINE METRICS SUMMARY                \n")
cat("==================================================\n")
cat(sprintf("Total Processed Ticks : %d\n", total_ticks))
cat(sprintf("Total Anomalous Ticks : %d\n", anomalies))
cat(sprintf("Mean Latency          : %.3f microseconds\n", mean_latency))
cat(sprintf("95th Percentile (p95) : %.3f microseconds\n", p95_latency))
cat(sprintf("99th Percentile (p99) : %.3f microseconds\n", p99_latency))
cat(sprintf("Max Latency           : %.3f microseconds\n", max_latency))
cat("==================================================\n")

png("dashboard_plots.png", width=1200, height = 800, res = 120)
par(mfrow = c(3,1), mar  = c(4,4.5,2,1))

plot(metrics$latency_us, type = "o", pch = 20, col = "#2b8cbe",
     xlab = "Tick Index", ylab = "Latency (microseconds)",
     main = "Handoff Latency Timeline (Arrival to Processed)",
     cex.main = 1.1, font.main = 2, frame.plot = TRUE)
grid()

abline(h=20, col="#de2d26", lty = 2)

anom_indices <- which(metrics$validation_flags != 0)
plot(metrics$close, type = "l", col = "#31a354", lwd = 2,
     xlab = "Tick Index", ylab = "Close Price (USDT)",
     main = "Price Stream & Active Anomaly Flags",
     cex.main = 1.1, font.main = 2, frame.plot = TRUE)
grid()

if (length(anom_indices) > 0) {
  points(anom_indices, metrics$close[anom_indices], col = "#de2d26", pch = 18, cex = 1.5)
  legend("topleft", legend = c("Normal Price", "Anomaly Flagged"),
         col = c("#31a354", "#de2d26"), pch = c(NA, 18), lty = c(1, NA), lwd = c(2, NA))
}

hist(metrics$latency_us, breaks = 30, col = "#bcbddc", border = "#756bb1",
     xlab = "Latency (microseconds)", ylab = "Frequency",
     main = "Latency Distribution (Jitter Histogram)",
     cex.main = 1.1, font.main = 2)
grid(ny = TRUE, nx = FALSE)
dev.off()
cat("Dashboard generated and saved as 'dashboard_plots.png'\n")