import { ChevronLeft, ChevronRight } from "lucide-react"
import { Select } from "@/shared/ui/select"

interface PaginationProps {
  currentPage: number
  totalItems: number
  itemsPerPage: number
  onPageChange: (page: number) => void
  onItemsPerPageChange?: (size: number) => void
  itemLabel?: string
  pageSizeOptions?: number[]
}

export function Pagination({
  currentPage,
  totalItems,
  itemsPerPage,
  onPageChange,
  onItemsPerPageChange,
  itemLabel = "mục",
  pageSizeOptions = [10, 20, 50],
}: PaginationProps) {
  const totalPages = Math.ceil(totalItems / itemsPerPage)
  const startIndex = (currentPage - 1) * itemsPerPage
  const endIndex = Math.min(startIndex + itemsPerPage, totalItems)

  // Generate visible page numbers (show max 5 pages with ellipsis)
  const getPageNumbers = (): (number | "...")[] => {
    if (totalPages <= 5) {
      return Array.from({ length: totalPages }, (_, i) => i + 1)
    }

    const pages: (number | "...")[] = []

    if (currentPage <= 3) {
      pages.push(1, 2, 3, 4, "...")
    } else if (currentPage >= totalPages - 2) {
      pages.push("...", totalPages - 3, totalPages - 2, totalPages - 1, totalPages)
    } else {
      pages.push("...", currentPage - 1, currentPage, currentPage + 1, "...")
    }

    return pages
  }

  return (
    <div className="flex items-center justify-between px-4 py-3 border-t border-gray-200 bg-gray-50/30">
      <div className="text-sm text-gray-500">
        Hiển thị {totalItems > 0 ? startIndex + 1 : 0} đến {endIndex} trong tổng số {totalItems.toLocaleString()} {itemLabel}
      </div>
      <div className="flex items-center gap-2">
        {onItemsPerPageChange && (
          <Select
            className="w-[100px] h-8 text-sm"
            value={String(itemsPerPage)}
            onChange={(e) => onItemsPerPageChange(Number(e.target.value))}
            options={pageSizeOptions.map((n) => ({
              value: String(n),
              label: `${n} / trang`,
            }))}
          />
        )}
        <div className="flex items-center gap-1">
          <button
            className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-500 hover:bg-gray-100 transition-colors disabled:opacity-40 disabled:cursor-not-allowed"
            disabled={currentPage <= 1}
            onClick={() => onPageChange(currentPage - 1)}
          >
            <ChevronLeft className="h-4 w-4" />
          </button>

          {getPageNumbers().map((page, idx) =>
            page === "..." ? (
              <span key={`ellipsis-${idx}`} className="px-1 text-gray-400">
                ...
              </span>
            ) : (
              <button
                key={page}
                className={`w-8 h-8 flex items-center justify-center rounded-lg text-sm font-medium transition-colors ${
                  page === currentPage
                    ? "bg-[#D9272B] text-white"
                    : "border border-gray-200 text-gray-700 hover:bg-gray-100"
                }`}
                onClick={() => onPageChange(page)}
              >
                {page}
              </button>
            )
          )}

          <button
            className="w-8 h-8 flex items-center justify-center rounded-lg border border-gray-200 text-gray-500 hover:bg-gray-100 transition-colors disabled:opacity-40 disabled:cursor-not-allowed"
            disabled={currentPage >= totalPages}
            onClick={() => onPageChange(currentPage + 1)}
          >
            <ChevronRight className="h-4 w-4" />
          </button>
        </div>
      </div>
    </div>
  )
}