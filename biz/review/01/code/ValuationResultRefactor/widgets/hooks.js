import { useEffect } from 'react'
import { usePost } from 'rootnet-biz/lib/hooks'

export function curryUseGetData(url) {
  return function useGetData(params) {
    const { data, doFetch, loading, error } = usePost()

    useEffect(() => {
      doFetch(url, params)
    }, [doFetch, params])

    return { data, doFetch, loading, error }
  }
}
