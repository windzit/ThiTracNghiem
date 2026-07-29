/**
 * Utility for minimum visible spinner duration (prevents flash-of-spinner on fast local connections)
 * Ensures a promise takes at least `ms` milliseconds to resolve.
 */
export const minDelay = <T>(promise: Promise<T>, ms = 400): Promise<T> =>
  Promise.all([promise, new Promise((resolve) => setTimeout(resolve, ms))]).then(([res]) => res)
